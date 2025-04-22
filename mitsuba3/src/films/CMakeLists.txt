set(MI_PLUGIN_PREFIX "films")

add_plugin(hdrfilm  hdrfilm.cpp)
add_plugin(specfilm  specfilm.cpp)
add_plugin(rsfilm  rsfilm.cpp)
add_plugin(rsspecfilm  rsspecfilm.cpp)

set(MI_PLUGIN_TARGETS "${MI_PLUGIN_TARGETS}" PARENT_SCOPE)
