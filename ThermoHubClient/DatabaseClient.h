// Copyright (C) 2019 G. Dan Miron
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
     * @brief Save Database to json file
     * 
     * @param thermodataset 
     * @param fileName name of the local file (default relative path Resources/databases/)
     */
    auto saveDatabase(const std::string &thermodataset, const std::string &fileName) -> void;

private:
    struct Impl;

    std::shared_ptr<Impl> pimpl;
};

} // namespace ThermoHubClient