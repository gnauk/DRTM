#include <mitsuba/core/properties.h>
#include <mitsuba/core/spectrum.h>
#include <mitsuba/core/warp.h>
#include <mitsuba/render/bsdf.h>
#include <mitsuba/render/texture.h>

NAMESPACE_BEGIN(mitsuba)

/**!

.. _bsdf-difftrans:

Smooth difftrans material (:monosp:`difftrans`)
-------------------------------------------

.. pluginparameters::

 * - transmittance
   - |spectrum| or |texture|
   - Specifies the diffuse albedo of the material (Default: 0.5)
   - |exposed|, |differentiable|

This BSDF models a non-reflective material, where any entering light loses
its directionality and is diffusely scattered from the other side. This
model can be combined\footnote{For instance using the
\pluginref{mixturebsdf} plugin.} with a surface reflection model to
describe translucent substances that have internal multiple scattering
processes (e.g. plant leaves).

.. tabs::
    .. code-tab:: xml
        :name: difftrans-srgb

        <bsdf type="difftrans">
            <rgb name="transmittance" value="0.2, 0.25, 0.7"/>
        </bsdf>

    .. code-tab:: python

        'type': 'difftrans',
        'transmittance': {
            'type': 'rgb',
            'value': [0.2, 0.25, 0.7]
        }

Alternatively, the reflectance can be textured:

.. tabs::
    .. code-tab:: xml
        :name: difftrans-texture

        <bsdf type="difftrans">
            <texture type="bitmap" name="transmittance">
                <string name="filename" value="wood.jpg"/>
            </texture>
        </bsdf>

    .. code-tab:: python

        'type': 'difftrans',
        'transmittance': {
            'type': 'bitmap',
            'filename': 'wood.jpg'
        }
*/
template <typename Float, typename Spectrum>
class DiffuseTransmitter final : public BSDF<Float, Spectrum> {
public:
    MI_IMPORT_BASE(BSDF, m_flags, m_components)
    MI_IMPORT_TYPES(Texture)

    DiffuseTransmitter(const Properties &props) : Base(props) {
        m_transmittance = props.texture<Texture>("transmittance", .5f);
        m_flags = BSDFFlags::DiffuseTransmission | BSDFFlags::FrontSide | BSDFFlags::BackSide;
        dr::set_attr(this, "flags", m_flags);
        m_components.push_back(m_flags);
    }

    void traverse(TraversalCallback *callback) override {
        callback->put_object("transmittance", m_transmittance.get(), +ParamFlags::Differentiable);
    }

    std::pair<BSDFSample3f, Spectrum> sample(const BSDFContext &ctx,
                                             const SurfaceInteraction3f &si,
                                             Float /* sample1 */,
                                             const Point2f &sample2,
                                             Mask active) const override {
        MI_MASKED_FUNCTION(ProfilerPhase::BSDFSample, active);

        Float cos_theta_i = Frame3f::cos_theta(si.wi);
        BSDFSample3f bs = dr::zeros<BSDFSample3f>();

        //active &= cos_theta_i > 0.f;
        if (!ctx.is_enabled(BSDFFlags::DiffuseTransmission))
            return { bs, 0.f };

        bs.wo = warp::square_to_cosine_hemisphere(sample2);
        //if (cos_theta_i > 0)
        //    bs.wo.z *= -1;
        Mask m0 = active && cos_theta_i > 0;
        if (dr::any_or<true>(m0)) {
            bs.wo[2] *= -1;
        }
        bs.pdf = dr::abs(Frame3f::cos_theta(bs.wo)) * dr::InvPi<Float>;
        bs.eta = 1.f;
        bs.sampled_type = +BSDFFlags::DiffuseTransmission;
        bs.sampled_component = 0;

        UnpolarizedSpectrum value = m_transmittance->eval(si, active);

        return { bs, depolarizer<Spectrum>(value) & (active && bs.pdf > 0.f) };
    }

    Spectrum eval(const BSDFContext &ctx, const SurfaceInteraction3f &si,
                  const Vector3f &wo, Mask active) const override {
        MI_MASKED_FUNCTION(ProfilerPhase::BSDFEvaluate, active);

        Float cos_theta_i = Frame3f::cos_theta(si.wi),
              cos_theta_o = Frame3f::cos_theta(wo);

        active &= (cos_theta_i * cos_theta_o < 0.f);

        if (unlikely(dr::none_or<false>(active) || 
                     !ctx.is_enabled(BSDFFlags::DiffuseTransmission)))
            return 0.f;


        UnpolarizedSpectrum value =
            m_transmittance->eval(si, active) * dr::InvPi<Float> * dr::abs(cos_theta_o);

        return depolarizer<Spectrum>(value) & active;
    }

    Float pdf(const BSDFContext &ctx, const SurfaceInteraction3f &si,
              const Vector3f &wo, Mask active) const override {
        MI_MASKED_FUNCTION(ProfilerPhase::BSDFEvaluate, active);


        Float cos_theta_i = Frame3f::cos_theta(si.wi),
              cos_theta_o = Frame3f::cos_theta(wo);

        active &= (cos_theta_i*cos_theta_o < 0.f);
        if (unlikely(dr::none_or<false>(active) || 
                     !ctx.is_enabled(BSDFFlags::DiffuseTransmission)))
            return 0.f;


        Float pdf = dr::abs(Frame3f::cos_theta(wo)) * dr::InvPi<Float>;

        return dr::select(cos_theta_i*cos_theta_o < 0.f, pdf, 0.f);
    }

    std::pair<Spectrum, Float> eval_pdf(const BSDFContext &ctx,
                                        const SurfaceInteraction3f &si,
                                        const Vector3f &wo,
                                        Mask active) const override {
        MI_MASKED_FUNCTION(ProfilerPhase::BSDFEvaluate, active);

        Float cos_theta_i = Frame3f::cos_theta(si.wi),
              cos_theta_o = Frame3f::cos_theta(wo);

        active &= (cos_theta_i * cos_theta_o < 0.f);
         if (unlikely(dr::none_or<false>(active) || 
                     !ctx.is_enabled(BSDFFlags::DiffuseTransmission)))
            return { 0.f, 0.f };


        UnpolarizedSpectrum value =
            m_transmittance->eval(si, active) * dr::InvPi<Float> * dr::abs(cos_theta_o);

        Float pdf = dr::abs(cos_theta_o) * dr::InvPi<Float>;

        return { depolarizer<Spectrum>(value) & active, dr::select(active, pdf, 0.f) };
    }

    Spectrum eval_diffuse_reflectance(const SurfaceInteraction3f &si,
                                      Mask active) const override {
        return m_transmittance->eval(si, active);
    }

    std::string to_string() const override {
        std::ostringstream oss;
        oss << "DiffuseTransmitter[" << std::endl
            << "  transmittance = " << string::indent(m_transmittance) << std::endl
            << "]";
        return oss.str();
    }

    MI_DECLARE_CLASS()
private:
    ref<Texture> m_transmittance;
};

MI_IMPLEMENT_CLASS_VARIANT(DiffuseTransmitter, BSDF)
MI_EXPORT_PLUGIN(DiffuseTransmitter, "difftrans material")
NAMESPACE_END(mitsuba)
