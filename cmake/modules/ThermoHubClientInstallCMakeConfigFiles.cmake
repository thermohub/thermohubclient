# The path where cmake config files are installed
set(THERMOHUBCLIENT_INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/ThermoHubClient)

install(EXPORT ThermoHubClientTargets
    FILE ThermoHubClientTargets.cmake
    NAMESPACE ThermoHubClient::
    DESTINATION ${THERMOHUBCLIENT_INSTALL_CONFIGDIR}
    COMPONENT cmake)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/ThermoHubClientConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/ThermoHubClientConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/ThermoHubClientConfig.cmake
    INSTALL_DESTINATION ${THERMOHUBCLIENT_INSTALL_CONFIGDIR}
    PATH_VARS THERMOHUBCLIENT_INSTALL_CONFIGDIR)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/ThermoHubClientConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/ThermoHubClientConfigVersion.cmake
    DESTINATION ${THERMOHUBCLIENT_INSTALL_CONFIGDIR})
