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
                  "Get database JSON string for a given ThermoDataSet symbol", "thermodataset")
        // .def("getDatabase", (const std::string& (DatabaseClient::*)(const std::string&, const std::vector<std::string>&) const) &DatabaseClient::getDatabase,
                //   "Get database JSON string for a given ThermoDataSet symbol and a list of elements", "thermodataset", "elements"/*={}*/)
        .def("saveDatabase", (void (DatabaseClient::*)(const std::string&)) &DatabaseClient::saveDatabase,
                  "Save thermodataset database to json file (name of thermodataset)", "thermodataset")
        // .def("saveDatabase", (void (DatabaseClient::*)(const std::string&, const std::vector<std::string>&)) &DatabaseClient::saveDatabase,
                //   "Save thermodataset, filtered with a given list of elements to json file (name of thermodataset)", "thermodataset", "elements"/*={}*/)
        .def("availableThermoDataSets", &DatabaseClient::availableThermoDataSets,"list of available ThermoDataSets")
        ;
}
}
