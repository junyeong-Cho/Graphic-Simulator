include(FetchContent)
set(FETCHCONTENT_QUIET TRUE)
set(CMAKE_VERBOSE_MAKEFILE OFF)
if (CMAKE_VERSION VERSION_GREATER "3.24.0")
    # Avoid warnings in CMake 3.24.0:
    cmake_policy(SET CMP0135 NEW)
endif()

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE
      Release
      CACHE STRING "Choose the type of build." FORCE)
  
endif()

if(NOT CMAKE_CONFIGURATION_TYPES)
# Set the possible values of build type for cmake-gui, ccmake
  set_property(
    CACHE CMAKE_BUILD_TYPE
    PROPERTY STRINGS
             "Debug"
             "Release"
             "MinSizeRel"
             "RelWithDebInfo")
endif()

# Generate compile_commands.json to make it easier to work with clang based tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
