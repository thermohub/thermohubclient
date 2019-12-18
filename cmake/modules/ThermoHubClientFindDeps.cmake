find_package(Boost COMPONENTS regex REQUIRED)

#find_package(curl REQUIRED)
if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
find_library(CURL_LIB libcurl)
else()
find_library(CURL_LIB curl)
endif()
if(NOT CURL_LIB)
  message(FATAL_ERROR "curl library not found")
endif()

#find_package(velocypack REQUIRED)
find_library(VELOCYPACK_LIB velocypack)
if(NOT VELOCYPACK_LIB)
  message(FATAL_ERROR "velocypack library not found")
endif()

#find_package(jsonio REQUIRED)
find_library(JSONARANGO_LIB jsonarango)
if(NOT JSONARANGO_LIB)
  message(FATAL_ERROR "jsonarango library not found")
endif()

# Find pybind11 library (if needed)
if(THERMOHUBCLIENT_BUILD_PYTHON)
    find_package(pybind11 REQUIRED)
    if(NOT pybind11_FOUND)
        message(WARNING "Could not find pybind11 - the Python module `thermofun` will not be built.")
        set(THERMOHUBCLIENT_BUILD_PYTHON OFF)
    else()
        message(STATUS "Found pybind11 v${pybind11_VERSION}: ${pybind11_INCLUDE_DIRS}")
    endif()
endif()
