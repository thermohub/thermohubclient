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

// C++ includes
#include <string>
#include <memory>
#include <vector>

namespace ThermoHubClient
{

class DatabaseClient
{
public:
    DatabaseClient();

    DatabaseClient(const std::string& connection_configuration);

    /// Assign a DatabaseClient instance to this instance
    auto operator=(DatabaseClient other) -> DatabaseClient &;

    /// Destroy this instance
    virtual ~DatabaseClient();

     /**
     * @brief Get the  Database
     * 
     * @param thermodataset symbol of ThermoDataSet available in ThermoHub server (local or remote)
     * @return JSON string {...}  
     */
    auto getDatabase(const std::string &thermodataset) const -> const std::string&;

    /**
     * @brief Get the Database object
     * 
     * @param thermodataset symbol of thermodataset from the database
     * @param elements list of elements to filter selected data
     * @return const std::string& 
     */
    //auto getDatabase(const std::string &thermodataset, const std::vector<std::string>& elements) const -> const std::string&;

    /**
     * @brief Save Database to json file (<thermodataset>-thermofun.json)
     * 
     * @param thermodataset symbol of thermodataset from the database
     */
    auto saveDatabase(const std::string &thermodataset) -> void;

    /**
     * @brief Save Database to json file (<thermodataset>-thermofun.json)
     * 
     * @param thermodataset symbol of thermodataset from the database
     * @param elements list of elements to filter selected data
     */
    //auto saveDatabase(const std::string &thermodataset, const std::vector<std::string>& elements) -> void;

    /**
     * @brief list of available ThermoDataSets
     * 
     * @return const std::vector<std::string>& 
     */
    auto availableThermoDataSets() -> std::vector<std::string>;

private:
    struct Impl;

    std::shared_ptr<Impl> pimpl;
};

} // namespace ThermoHubClient