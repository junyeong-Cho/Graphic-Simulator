FetchContent_Declare(
    the_gsl
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    URL https://github.com/microsoft/GSL/archive/refs/tags/v4.0.0.tar.gz
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
)
FetchContent_MakeAvailable(the_gsl)

add_library(gsl INTERFACE)
target_include_directories(gsl SYSTEM INTERFACE ${the_gsl_SOURCE_DIR}/include)
