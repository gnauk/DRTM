
## Introduction

Mitsuba 3 is a research-oriented rendering system for forward and inverse light
transport simulation developed at [EPFL](https://www.epfl.ch) in Switzerland.
It consists of a core library and a set of plugins that implement functionality
ranging from materials and light sources to complete rendering algorithms.

Mitsuba 3 is *retargetable*: this means that the underlying implementations and
data structures can transform to accomplish various different tasks. For
example, the same code can simulate both scalar (classic one-ray-at-a-time) RGB transport
or differential spectral transport on the GPU. This all builds on
[Dr.Jit](https://github.com/mitsuba-renderer/drjit), a specialized *just-in-time*
(JIT) compiler developed specifically for this project.

## Main Features

- **Cross-platform**: Mitsuba 3 has been tested on Linux (``x86_64``), macOS
  (``aarch64``, ``x86_64``), and Windows (``x86_64``).

- **High performance**: The underlying Dr.Jit compiler fuses rendering code
  into kernels that achieve state-of-the-art performance using
  an LLVM backend targeting the CPU and a CUDA/OptiX backend
  targeting NVIDIA GPUs with ray tracing hardware acceleration.

- **Python first**: Mitsuba 3 is deeply integrated with Python. Materials,
  textures, and even full rendering algorithms can be developed in Python,
  which the system JIT-compiles (and optionally differentiates) on the fly.
  This enables the experimentation needed for research in computer graphics and
  other disciplines.

- **Differentiation**: Mitsuba 3 is a differentiable renderer, meaning that it
  can compute derivatives of the entire simulation with respect to input
  parameters such as camera pose, geometry, BSDFs, textures, and volumes. It
  implements recent differentiable rendering algorithms developed at EPFL.

- **Spectral & Polarization**: Mitsuba 3 can be used as a monochromatic
  renderer, RGB-based renderer, or spectral renderer. Each variant can
  optionally account for the effects of polarization if desired.

## Tutorial videos, documentation

We've recorded several [YouTube videos][10] that provide a gentle introduction
Mitsuba 3 and Dr.Jit. Beyond this you can find complete Juypter notebooks
covering a variety of applications, how-to guides, and reference documentation
on [readthedocs][2].

## Installation

We provide pre-compiled binary wheels via PyPI. Installing Mitsuba this way is as simple as running

```bash
pip install mitsuba
```

on the command line. The Python package includes four variants by default:

- ``scalar_spectral``
- ``scalar_rgb``
- ``llvm_ad_rgb``
- ``cuda_ad_rgb``

The first two perform classic one-ray-at-a-time simulation using either a RGB
or spectral color representation, while the latter two can be used for inverse
rendering on the CPU or GPU. To access additional variants, you will need to
compile a custom version of Dr.Jit using CMake. Please see the
[documentation](https://mitsuba.readthedocs.io/en/latest/src/developer_guide/compiling.html)
for details on this.

### Requirements

- `Python >= 3.8`
- (optional) For computation on the GPU: `Nvidia driver >= 495.89`
- (optional) For vectorized / parallel computation on the CPU: `LLVM >= 11.1`

## Usage

Here is a simple "Hello World" example that shows how simple it is to render a
scene using Mitsuba 3 from Python:

```python
# Import the library using the alias "mi"
import mitsuba as mi
# Set the variant of the renderer
mi.set_variant('scalar_rgb')
# Load a scene
scene = mi.load_dict(mi.cornell_box())
# Render the scene
img = mi.render(scene)
# Write the rendered image to an EXR file
mi.Bitmap(img).write('cbox.exr')
```

Tutorials and example notebooks covering a variety of applications can be found
in the [documentation][2].

## About

This project was created by [Wenzel Jakob](https://rgl.epfl.ch/people/wjakob).
Significant features and/or improvements to the code were contributed by
[Sébastien Speierer](https://speierers.github.io/),
[Nicolas Roussel](https://github.com/njroussel),
[Merlin Nimier-David](https://merlin.nimierdavid.fr/),
[Delio Vicini](https://dvicini.github.io/),
[Tizian Zeltner](https://tizianzeltner.com/),
[Baptiste Nicolet](https://bnicolet.com/),
[Miguel Crespo](https://mcrespo.me/),
[Vincent Leroy](https://github.com/leroyvn), and
[Ziyi Zhang](https://github.com/ziyi-zhang).

When using Mitsuba 3 in academic projects, please cite:

```bibtex
@software{Mitsuba3,
    title = {Mitsuba 3 renderer},
    author = {Wenzel Jakob and Sébastien Speierer and Nicolas Roussel and Merlin Nimier-David and Delio Vicini and Tizian Zeltner and Baptiste Nicolet and Miguel Crespo and Vincent Leroy and Ziyi Zhang},
    note = {https://mitsuba-renderer.org},
    version = {3.1.1},
    year = 2022
}
```
