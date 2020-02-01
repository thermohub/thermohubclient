# Recursively collect all header files from the current directory
file(GLOB_RECURSE HEADER_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.hpp *.h)

# Recursively collect all source files from the current directory
file(GLOB_RECURSE SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

# The name of the shared and static libraries
set(THERMOHUBCLIENT_SHARED_LIB ${PROJECT_NAME}${SUFFIX_SHARED_LIBS})
set(THERMOHUBCLIENT_STATIC_LIB ${PROJECT_NAME}${SUFFIX_STATIC_LIBS})

# Set the list of names of the third-party targets and libraries
# set(THIRDPARTY_LIBS jsonio boost_regex boost_filesystem curl velocypack)

# Enable automatic creation of a module definition (.def) file for a SHARED library on Windows.
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

# Build a ThermoHubClient C++ library
add_library(ThermoHubClient ${HEADER_FILES} ${SOURCE_FILES})

# Add an alias ThermoHubClient::ThermoHubClient to the target library ThermoHubClient
add_library(ThermoHubClient::ThermoHubClient ALIAS ThermoHubClient)

# Add the include paths to the ThermoHubClient target
target_include_directories(ThermoHubClient
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# Link ThermoHubClient library against external dependencies
target_link_libraries(ThermoHubClient
    PRIVATE ${THIRDPARTY_LIBS}
    PUBLIC velocypack
    PUBLIC jsonarango-static
    )

if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
# Link ThermoHubClient library against external (linked in static jsonarango) dependencies
target_link_libraries(ThermoHubClient
    PUBLIC libcurl
    )
else()
target_link_libraries(ThermoHubClient
    PUBLIC curl
    )
endif()

# Set compilation features to be propagated to dependent codes.
# target_compile_features(ThermoHubClient PUBLIC cxx_std_17)

# Set some target properties
set_target_properties(ThermoHubClient PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    CXX_EXTENSIONS OFF)

# Install ThermoHubClient C++ library
install(TARGETS ThermoHubClient
    EXPORT ThermoHubClientTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libraries
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libraries
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT libraries)

# Install debug symbols
 if(MSVC)
     install(
         FILES $<TARGET_PDB_FILE:ThermoHubClient>
         DESTINATION ${CMAKE_INSTALL_BINDIR}
         COMPONENT libraries
         OPTIONAL
     )
 endif()

# Install ThermoHubClient header files
install(DIRECTORY ${PROJECT_SOURCE_DIR}/ThermoHubClient
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT headers
    FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")

# Install deps header files
#install(DIRECTORY ${PROJECT_SOURCE_DIR}/ThermoHubClient/deps
#    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ThermoHubClient COMPONENT headers)