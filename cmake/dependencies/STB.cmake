FetchContent_Declare(
    stb_github
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
    GIT_SHALLOW TRUE
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
)
FetchContent_MakeAvailable(stb_github)
target_include_directories(project_options SYSTEM INTERFACE ${stb_github_SOURCE_DIR})

if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/stb_implementation.cpp)

    # Multiline string with the STB image implementation
    set(STB_IMAGE_IMPLEMENTATION_CODE " // This file is auto-generated from cmake/dependencies/STB.cmake
#define STB_IMAGE_IMPLEMENTATION
#include \"stb_image.h\"
// #define STB_IMAGE_WRITE_IMPLEMENTATION
// #include \"stb_image_write.h\"
")

    # Generate an implementation file
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/stb_implementation.cpp "${STB_IMAGE_IMPLEMENTATION_CODE}")

endif()

add_library(stb STATIC ${CMAKE_CURRENT_BINARY_DIR}/stb_implementation.cpp)
target_include_directories(stb SYSTEM PUBLIC ${stb_github_SOURCE_DIR})
