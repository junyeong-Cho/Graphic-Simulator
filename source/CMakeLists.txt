set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(SOURCE_CODE

    assets/Path.hpp assets/Path.cpp
    assets/Reloader.hpp assets/Reloader.cpp

    demos/D01HelloQuad.hpp demos/D01HelloQuad.cpp
    demos/D02ProceduralMeshes.hpp demos/D02ProceduralMeshes.cpp
    demos/DemosFactory.hpp demos/DemosFactory.cpp
    demos/IDemo.hpp

    environment/Environment.hpp
    environment/Input.hpp
    environment/OpenGL.hpp

    graphics/Material.hpp graphics/Material.cpp
    graphics/Mesh.hpp graphics/Mesh.cpp

    opengl/GL.hpp opengl/GL.cpp
    opengl/GLHandle.hpp
    opengl/GLIndexBuffer.hpp opengl/GLIndexBuffer.cpp
    opengl/GLShader.hpp opengl/GLShader.cpp
    opengl/GLTexture.hpp opengl/GLTexture.cpp
    opengl/GLVertexArray.hpp opengl/GLVertexArray.cpp
    opengl/GLVertexBuffer.hpp opengl/GLVertexBuffer.cpp

    util/FPS.hpp
    util/Timer.hpp
    util/WatchFiles.hpp util/WatchFiles.cpp

    window/Application.hpp window/Application.cpp
    window/ImGuiHelper.hpp window/ImGuiHelper.cpp
    window/Logo.hpp window/Logo.cpp
    window/Settings.hpp window/Settings.cpp

    main.cpp
)

set(GRAPHICS_FUN_LINK_OPTIONS "")

if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    # https://emscripten.org/docs/tools_reference/settings_reference.html
    # ASSERTIONS=1                  - we want asserts to work
    # WASM=1                        - we want web assembly generated rather than just javascript
    # DISABLE_EXCEPTION_CATCHING=0  - we are using exceptions so let's not disable them
    # ALLOW_MEMORY_GROWTH=1         - we don't know exactly how much heap space we'll need so allow it to grow
    # EXIT_RUNTIME=1                - have exiting actually stop the program
    # SINGLE_FILE=1                 - generate everything into one html file
    # --embed-file                  - we need to tell it what files we will be reading at runtime. We are mapping the assets folder to the root folder
    # --use-preload-cache           - help with faster reloads : https://emscripten.org/docs/compiling/Deploying-Pages.html#providing-a-quick-second-time-load
    # -lembind                      - to call c++ from javascript https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html
    # --shell-file                  - to customize the webpage https://emscripten.org/docs/compiling/Deploying-Pages.html#build-files-and-custom-shell
    set(GRAPHICS_FUN_LINK_OPTIONS 
        -sASSERTIONS=1 
        -sWASM=1 
        -sNO_DISABLE_EXCEPTION_CATCHING 
        -sALLOW_MEMORY_GROWTH=1 
        -sEXIT_RUNTIME=1 
        -sSINGLE_FILE=1 
        --embed-file ${CMAKE_SOURCE_DIR}/assets@/assets
        --use-preload-cache
        -lembind
        $<$<CONFIG:Release>:--shell-file ${CMAKE_SOURCE_DIR}/app_resources/web/index_shell.html>
    )
endif()

add_executable(graphics_fun ${SOURCE_CODE})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_CODE})

target_link_libraries(graphics_fun PRIVATE project_options dependencies)
target_include_directories(graphics_fun PRIVATE .)
target_compile_definitions(graphics_fun PRIVATE $<$<NOT:$<CONFIG:Release>>:DEVELOPER_VERSION>)
target_link_options(graphics_fun PRIVATE ${GRAPHICS_FUN_LINK_OPTIONS})

install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} DESTINATION ${CMAKE_BINARY_DIR}/install)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR}/install)
