FetchContent_Declare(
    the_glm
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    URL https://github.com/g-truc/glm/releases/download/1.0.0/glm-1.0.0-light.7z
    SOURCE_DIR _deps/the_glm-src/glm
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
)

FetchContent_MakeAvailable(the_glm)

add_library(cs250_glm INTERFACE)
target_include_directories(cs250_glm SYSTEM INTERFACE ${the_glm_SOURCE_DIR}/..)
