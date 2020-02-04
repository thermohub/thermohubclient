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

struct DatabaseClientOptions
{
    // number of spaces in the json indentation
    int json_indent = 2;
    // number of spaces in the json indentation (in string returned by the get functions)
    int json_indent_get = -1;
    // filter charge when selecting data by elements
    bool filterCharge = false;
    // database filename suffix
    std::string databaseFileSuffix = "-thermofun";
    // subset database file suffix, when saving a subset of a ThermoDataSet based on a
    // list of elements, substances, aggregate state
    std::string subsetFileSuffix = "-subset-thermofun";
};

class DatabaseClient
{
public:
    DatabaseClient();

    DatabaseClient(const std::string &connection_configuration);

    /// Assign a DatabaseClient instance to this instance
    auto operator=(DatabaseClient other) -> DatabaseClient &;

    /// Destroy this instance
    virtual ~DatabaseClient();

    /**
     * @brief Get the  Database
     * 
     * @param thermodataset symbol of ThermoDataSet available in ThermoHub server (local or remote)
     * @return const std::string& JSON string {...} 
     */
    auto getDatabase(const std::string &thermodataset) const -> const std::string &;

    /**
     * @brief Get the Database Subset JSON string
     * 
     * @param thermodataset symbol of ThermoDataSet available in ThermoHub server (local or remote)
     * @param elements vector of elements symbols (optional)
     * @param substances vector of substances symbols (optional)
     * @param classes vector of substances classes (optional)
     * @param aggregatestates vector of substances aggregate states (optional)
     * @return const std::string& JSON string {...} 
     */
    auto getDatabaseSubset(const std::string &thermodataset, const std::vector<std::string> &elements = {},
                           const std::vector<std::string> &substances = {},
                           const std::vector<std::string> &classesOfSubstance = {},
                           const std::vector<std::string> &aggregateStates = {}) const -> const std::string &;

    /**
     * @brief Get the Database object
     * 
     * @param thermodataset symbol of thermodataset from the database
     * @param elements list of elements to filter selected data
     * @return const std::string& 
     */
    auto getDatabaseContainingElements(const std::string &thermodataset, const std::vector<std::string> &elements) const -> const std::string &;

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
    auto saveDatabaseContainingElements(const std::string &thermodataset, const std::vector<std::string> &elements) -> void;

    /**
     * @brief Save Database subset to json file (<thermodataset>-subset-thermofun.json)
     * 
     * @param thermodataset symbol of ThermoDataSet available in ThermoHub server (local or remote)
     * @param elements vector of elements symbols (optional)
     * @param substances vector of substances symbols (optional)
     * @param classes vector of substances classes (optional)
     * @param aggregatestates vector of substances aggregate states (optional)
     */
    auto saveDatabaseSubset(const std::string &thermodataset, const std::vector<std::string> &elements = {},
                            const std::vector<std::string> &substances = {},
                            const std::vector<std::string> &classesOfSubstance = {},
                            const std::vector<std::string> &aggregateStates = {}) -> void;
    /**
     * @brief list of available ThermoDataSets
     * 
     * @return std::vector<std::string> 
     */
    auto availableThermoDataSets() -> std::vector<std::string>;

    /**
     * @brief list of substance classes in a ThermoDataSet
     * 
     * @param thermodataset symbol of ThermoDataSet available in ThermoHub server (local or remote) 
     * @return std::vector<std::string> 
     */
    auto substanceClassesInThermoDataSet(const std::string &thermodataset) -> std::vector<std::string>;

    /**
     * @brief list of substance aggregate states in a ThermoDataSet
     * 
     * @param thermodataset symbol of ThermoDataSet available in ThermoHub server (local or remote)
     * @return std::vector<std::string> 
     */
    auto substanceAggregateStatesInThermoDataSet(const std::string &thermodataset) -> std::vector<std::string>;

    /**
     * @brief list of elements in a ThermoDataSet
     * 
     * @param thermodataset symbol of ThermoDataSet available in ThermoHub server (local or remote)
     * @return std::vector<std::string> 
     */
    auto elementsInThermoDataSet(const std::string &thermodataset) -> std::vector<std::string>;

    /**
     * @brief list of substances in a ThermoDataSet
     * 
     * @param thermodataset symbol of ThermoDataSet available in ThermoHub server (local or remote)
     * @return std::vector<std::string> 
     */
    auto substancesInThermoDataSet(const std::string &thermodataset) -> std::vector<std::string>;

    /**
     * @brief list of reactions in a ThermoDataSet
     * 
     * @param thermodataset symbol of ThermoDataSet available in ThermoHub server (local or remote)
     * @return std::vector<std::string> 
     */
    auto reactionsInThermoDataSet(const std::string &thermodataset) -> std::vector<std::string>;

private:
    struct Impl;

    std::shared_ptr<Impl> pimpl;
};

} // namespace ThermoHubClient