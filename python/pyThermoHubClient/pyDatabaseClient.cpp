// Copyright (C) 2020 G. D. Miron, D. A. Kulik, S. V Dmytrieva
// 
// thermohubclient is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// thermohubclient is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with thermohubclient. If not, see <http://www.gnu.org/licenses/>.

// pybind11 includes
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

// ThermoFun includes
#include <ThermoHubClient/DatabaseClient.h>

namespace ThermoHubClient {

void exportDatabaseClient(py::module& m)
{
    py::class_<DatabaseClient>(m, "DatabaseClient")
        .def(py::init<>())
        .def(py::init<const std::string&>())
        .def("getDatabase", (const std::string& (DatabaseClient::*)(const std::string&) const) &DatabaseClient::getDatabase,
                  "Get thermodataset database JSON string for a given ThermoDataSet symbol", "thermodataset")
        .def("getDatabaseContainingElements", (const std::string& (DatabaseClient::*)(const std::string&) const) &DatabaseClient::getDatabaseContainingElements,
                  "Get thermodataset database JSON string for a given ThermoDataSet symbol and a list of elements", "thermodataset", "elements")
        .def("getDatabaseSubset", &DatabaseClient::getDatabaseSubset,
                  "Get thermodataset database JSON string for a given ThermoDataSet symbol and optional a list of elements, substances, substance classes, substance aggregate states",
                  py::arg("thermodataset"), py::arg("elements") = std::vector<std::string>(), py::arg("substances") = std::vector<std::string>(), 
                  py::arg("classesOfSubstance") = std::vector<std::string>(), py::arg("aggregateStates") = std::vector<std::string>())
        .def("saveDatabase", (void (DatabaseClient::*)(const std::string&)) &DatabaseClient::saveDatabase,
                  "Save thermodataset database to JSON file, for a given ThermoDataSet symbol", "thermodataset")
        .def("saveDatabaseContainingElements", (const std::string& (DatabaseClient::*)(const std::string&) const) &DatabaseClient::getDatabaseContainingElements,
                  "Save thermodataset database to JSON file, for a given ThermoDataSet symbol and a list of elements", "thermodataset", "elements")
        .def("saveDatabaseSubset", &DatabaseClient::saveDatabaseSubset,
                  "Save subset thermodataset database to a JSON file for a given ThermoDataSet symbol and optional a list of elements, substances, substance classes, substance aggregate states",
                  py::arg("thermodataset"), py::arg("elements") = std::vector<std::string>(), py::arg("substances") = std::vector<std::string>(), 
                  py::arg("classesOfSubstance") = std::vector<std::string>(), py::arg("aggregateStates") = std::vector<std::string>())
        .def("availableThermoDataSets", &DatabaseClient::availableThermoDataSets,"list of available ThermoDataSets", "thermodataset")
        .def("substanceClassesInThermoDataSet", &DatabaseClient::substanceClassesInThermoDataSet,"list of substance classes in a ThermoDataSet", "thermodataset")
        .def("substanceAggregateStatesInThermoDataSet", &DatabaseClient::substanceAggregateStatesInThermoDataSet,"list of substance aggregate states in ThermoDataSet", "thermodataset")
        .def("elementsInThermoDataSet", &DatabaseClient::elementsInThermoDataSet,"list of elements in a ThermoDataSet", "thermodataset")
        .def("substancesInThermoDataSet", &DatabaseClient::substancesInThermoDataSet,"list of substances in a ThermoDataSet", "thermodataset")
        .def("reactionsInThermoDataSet", &DatabaseClient::reactionsInThermoDataSet,"list of reactions in a ThermoDataSet", "thermodataset")        
        ;

}

void exportDatabaseClientOptions(py::module& m)
{
    py::class_<DatabaseClientOptions>(m, "DatabaseClientOptions")
        .def(py::init<>())
        .def_readwrite("json_indent", &DatabaseClientOptions::json_indent)
        .def_readwrite("filterCharge", &DatabaseClientOptions::filterCharge)
        .def_readwrite("databaseFileSuffix", &DatabaseClientOptions::databaseFileSuffix)
        .def_readwrite("subsetFileSuffix", &DatabaseClientOptions::subsetFileSuffix)
        ;
}
}
