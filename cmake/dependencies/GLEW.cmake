# We will use GLEW for OpenGL bindings
# Linux platforms have a very easy way to install these depenencies and expose them to compilers so,
#   we will use the builtin find_package
# Emscripten has their own port of GLEW builtin. We don't even need to specify any flags.
#   Windows does not have a simple way to get it, so we download official windows binaries and link against those
set(CS250_GLEW_INCLUDE_DIR "")
set(CS250_GLEW_TARGET_NAME "")
if (NOT WIN32 AND NOT EMSCRIPTEN)
    # on    Mac : brew install glew
    # on Ubuntu : apt install libglew-dev
    set(GLEW_USE_STATIC_LIBS TRUE)
    find_package(GLEW REQUIRED)
    set(CS250_GLEW_INCLUDE_DIR ${GLEW_INCLUDE_DIRS})
    set(CS250_GLEW_TARGET_NAME GLEW::glew_s)
elseif(WIN32)
    # download binaries for GLEW for windows x64
    FetchContent_Declare(
        glew
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        URL "https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0-win32.zip"
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
    )
    FetchContent_MakeAvailable(glew)
    set(CS250_GLEW_INCLUDE_DIR ${glew_SOURCE_DIR}/include)
    add_library(the_glew INTERFACE)
    target_link_directories(the_glew INTERFACE ${glew_SOURCE_DIR}/lib/Release/x64)
    target_link_libraries(the_glew INTERFACE glew32s) # note that is the name of the glew32s.lib file we need to link in
    set(CS250_GLEW_TARGET_NAME the_glew)
endif()

add_library(cs250_glew INTERFACE)
target_link_libraries(cs250_glew INTERFACE ${CS250_GLEW_TARGET_NAME} $<$<PLATFORM_ID:Linux>:GL>)
target_include_directories(cs250_glew SYSTEM INTERFACE ${CS250_GLEW_INCLUDE_DIR})
target_compile_definitions(cs250_glew INTERFACE GLEW_STATIC)
