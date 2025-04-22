set(MI_PLUGIN_PREFIX "textures")

add_plugin(bitmap         bitmap.cpp)
add_plugin(bitmap_polygon  bitmap_polygon.cpp)
add_plugin(bitmap_instance  bitmap_instance.cpp)
add_plugin(checkerboard   checkerboard.cpp)
add_plugin(mesh_attribute mesh_attribute.cpp)
add_plugin(volume         volume.cpp)

set(MI_PLUGIN_TARGETS "${MI_PLUGIN_TARGETS}" PARENT_SCOPE)
