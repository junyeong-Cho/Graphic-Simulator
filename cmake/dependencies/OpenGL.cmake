# all platforms should have OpenGL
find_package(OpenGL REQUIRED)
add_library(cs250_opengl INTERFACE)

target_link_libraries(cs250_opengl INTERFACE ${OPENGL_LIBRARIES})

if(EMSCRIPTEN)
    # We want WebGL 2
    # WebGL 2 is almost same as OpenGL ES 3 is almost same as OpenGL 3.3
    # MIN_WEBGL_VERSION=2           - Specifies the lowest WebGL version to target. Pass MIN_WEBGL_VERSION=2 to drop support for WebGL 1.0
    # MAX_WEBGL_VERSION=2           - Specifies the highest WebGL version to target. Pass -sMAX_WEBGL_VERSION=2 to enable targeting WebGL 2.
    target_link_options(cs250_opengl INTERFACE -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2)
    target_compile_definitions(cs250_opengl INTERFACE OPENGL_ES3_ONLY)
endif()
