# We will be using SDL2 for windowing & gl context creation
# Linux platforms have a very easy way to install these depenencies and expose them to compilers so,
#   we will use the builtin find_package
# Emscripten has their own port of SDL2 builtin. We can just #include, define USE_SDL=2, and start using it.
#   Windows does not have a simple way to get it, so we download official windows binaries and link against those

set(CS250_SDL2_INCLUDE_DIR "")
set(CS250_SDL2_TARGET_NAME "")
set(CS250_SDL2_COMPILE_OPTIONS "")
if (NOT WIN32 AND NOT EMSCRIPTEN)
    # on    Mac : brew install sdl2
    # on Ubuntu : apt install libsdl2-dev
    find_package(SDL2 REQUIRED)
    set(CS250_SDL2_INCLUDE_DIR ${SDL2_INCLUDE_DIRS})
    set(CS250_SDL2_TARGET_NAME ${SDL2_LIBRARIES})
elseif(EMSCRIPTEN)
    find_package(SDL2 REQUIRED)
    set(CS250_SDL2_TARGET_NAME ${SDL2_LIBRARIES})
    set(CS250_SDL2_INCLUDE_DIR ${SDL2_INCLUDE_DIRS})
    # USE_SDL=2                     - we want version 2 rather than SDL1
    set(CS250_SDL2_COMPILE_OPTIONS -sUSE_SDL=2)
else(WIN32)
    # download binaries for SDL2 for windows x64
    FetchContent_Declare(
        sdl2
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        URL "https://github.com/libsdl-org/SDL/releases/download/release-2.28.5/SDL2-devel-2.28.5-VC.zip"
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
    )
    FetchContent_MakeAvailable(sdl2)
    set(CS250_SDL2_INCLUDE_DIR ${sdl2_SOURCE_DIR}/include)
    # Define a custom target to copy SDL2.dll to the build directory
    # note this assumes CMAKE_RUNTIME_OUTPUT_DIRECTORY has already been set globally
    add_custom_target(copy_sdl2_dll
    COMMAND ${CMAKE_COMMAND} -E copy
            ${sdl2_SOURCE_DIR}/lib/x64/SDL2.dll
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/SDL2.dll
            DEPENDS sdl2 ${sdl2_SOURCE_DIR}/lib/x64/SDL2.dll
            COMMENT "Copying SDL2.dll to executable directory"
    )
    add_library(the_sdl2 INTERFACE)
    target_link_directories(the_sdl2 INTERFACE ${sdl2_SOURCE_DIR}/lib/x64)
    target_link_libraries(the_sdl2 INTERFACE SDL2) # note that SDL2 is the name of the SDL2.lib file we need to link in
    add_dependencies(the_sdl2 copy_sdl2_dll)
    set(CS250_SDL2_TARGET_NAME the_sdl2)
endif()

add_library(cs250_sdl2 INTERFACE)
target_link_libraries(cs250_sdl2 INTERFACE ${CS250_SDL2_TARGET_NAME})
target_include_directories(cs250_sdl2 SYSTEM INTERFACE ${CS250_SDL2_INCLUDE_DIR})
target_compile_options(cs250_sdl2 INTERFACE ${CS250_SDL2_COMPILE_OPTIONS})
