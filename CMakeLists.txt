# Require a certain version of cmake
cmake_minimum_required(VERSION 3.16)

# Set the name of the project
project(ThermoHubClient VERSION 0.2.0 LANGUAGES CXX)

# Set the cmake module path of the project
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

# Use ccache to speed up repeated compilations
include(CCache)

# Define variables with the GNU standard installation directories
include(GNUInstallDirs)

# Ensure proper configuration if in a conda environment
include(CondaAware)

# Define which types of libraries to build
option(THERMOHUBCLIENT_BUILD_SHARED_LIBS "Build shared libraries." ON)
option(THERMOHUBCLIENT_BUILD_STATIC_LIBS "Build static libraries." ON)
option(THERMOHUBCLIENT_BUILD_PYTHON "Build the python wrappers and python package thermohubclient." ON)
#option(REFRESH_THIRDPARTY "Refresh thirdparty libraries." OFF)

# Modify the HUBCLIENT_BUILD_* variables accordingly to BUILD_ALL
if(THERMOHUBCLIENT_BUILD_ALL MATCHES ON)
    set(THERMOHUBCLIENT_BUILD_SHARED_LIBS          ON)
    set(THERMOHUBCLIENT_BUILD_STATIC_LIBS          ON)
    set(THERMOHUBCLIENT_BUILD_PYTHON               ON)
endif()

# Set the default build type to Release
if(NOT CMAKE_BUILD_TYPE)
    # The build type selection for the project
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the build type for ${PROJECT_NAME}." FORCE)
    # The build type options for the project
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release MinSizeRel RelWithDebInfo)
endif()

# Define if shared library should be build instead of static.
option(BUILD_SHARED_LIBS "Build shared libraries." ON)

# Set libraries to be compiled with position independent code mode (i.e., fPIC option in GNU compilers)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the list of compiler flags for MSVC compiler
if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    add_compile_options(
        /D_SCL_SECURE_NO_WARNINGS
        /D_CRT_SECURE_NO_WARNINGS=1
        /MP4
        /EHsc
        /D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
        /DNOMINMAX
    )
endif()

# Set the ThermoHubClient source directory path
set(THERMOHUBCLIENT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/ThermoHubClient)

# Set the include directories
include_directories(${THERMOHUBCLIENT_SOURCE_DIR})

# Find all ThermoHubClient dependencies
include(ThermoHubClientFindDeps)

# Build ThermoHubClient library
add_subdirectory(ThermoHubClient)

# Build python wraper
if(THERMOHUBCLIENT_BUILD_PYTHON)
    add_subdirectory(python)
else()
#    add_subdirectory(python EXCLUDE_FROM_ALL)
endif()

# Install the cmake config files that permit users to use find_package(ThermoHubClient)
include(ThermoHubClientInstallCMakeConfigFiles)
