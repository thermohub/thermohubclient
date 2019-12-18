# Find pybind11
#find_package(pybind11 2.3.0 REQUIRED)

# Build the Python wrappers of ThermoHubClient library
add_subdirectory(pyThermoHubClient)

# Build the Python package for ThermoHubClient
add_subdirectory(thermohubclient)