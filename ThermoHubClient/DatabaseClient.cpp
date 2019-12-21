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

#include "DatabaseClient.h"
#include "AqlQueries.h"

// C++ includes
#include <regex>
#include <fstream>
#include <sstream>

// jsonarango
#include "jsonarango/arangocollection.h"
#include "jsonarango/arangoexception.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace ThermoHubClient
{

// Get Arangodb connection data( load settings from "examples-cfg.json" config file )
const arangocpp::ArangoDBConnection default_data(arangocpp::ArangoDBConnection::remote_server_endpoint, // server address
                                                 "funrem",                                              // username
                                                 "ThermoFun@Remote-ThermoHub-Server",                   // password
                                                 "hub_main");                                           // database name

struct DatabaseClient::Impl
{
    //    arangocpp::ArangoDBCollectionAPI connect{default_data};
    std::shared_ptr<arangocpp::ArangoDBCollectionAPI> dbClient;

    std::string queriedThermoDataSet;

    std::vector<std::string> recjsonValues;

    // Define call back function
    arangocpp::FetchingDocumentCallback collect_results_fn;

    auto setfunctions() -> void
    {
        collect_results_fn = [&](const std::string &jsondata) {
            recjsonValues.push_back(jsondata);
        };
    }

    // read from default config
    Impl()
    {
        setfunctions();

        try
        {
            // Create database connection
            arangocpp::ArangoDBCollectionAPI connect{default_data};
        }
        catch (arangocpp::arango_exception &e)
        {
            std::stringstream buffer;
            buffer << "ThermoHubClient" << e.header() << std::endl
                   << e.what() << std::endl;
            throw std::runtime_error(buffer.str());
        }
        catch (std::exception &e)
        {
            std::stringstream buffer;
            buffer << "ThermoHubClient"
                   << " std::exception" << e.what() << std::endl;
            throw std::runtime_error(buffer.str());
        }
        catch (...)
        {
            std::stringstream buffer;
            buffer << "ThermoHubClient"
                   << " unknown exception" << std::endl;
            throw std::runtime_error(buffer.str());
        }
    }

    Impl(const std::string &connection_configuration_file)
    {
        setfunctions();

        try
        {
            // Get Arangodb connection data( load settings from "examples-cfg.json" config file )
            arangocpp::ArangoDBConnection data = arangocpp::connectFromConfig(connection_configuration_file);
            // Create database connection
            arangocpp::ArangoDBCollectionAPI connect{data};
            dbClient = std::make_shared<arangocpp::ArangoDBCollectionAPI>(connect);
        }
        catch (arangocpp::arango_exception &e)
        {
            std::stringstream buffer;
            buffer << "ThermoHubClient" << e.header() << std::endl
                   << e.what() << std::endl;
            throw std::runtime_error(buffer.str());
        }
        catch (std::exception &e)
        {
            std::stringstream buffer;
            buffer << "ThermoHubClient"
                   << " std::exception" << e.what() << std::endl;
            throw std::runtime_error(buffer.str());
        }
        catch (...)
        {
            std::stringstream buffer;
            buffer << "ThermoHubClient"
                   << " unknown exception" << std::endl;
            throw std::runtime_error(buffer.str());
        }
    }

    auto idThermoDataSetFromSymbol(const std::string &symbol) -> std::string
    {

        recjsonValues.clear();

        std::string query = "FOR u  IN thermodatasets ";
        query += "FILTER u.properties.symbol == \"" + symbol + "\" ";
        query += "RETURN u._id";

        arangocpp::ArangoDBQuery aqlquery(query, arangocpp::ArangoDBQuery::AQL);
        dbClient->selectQuery("thermodatasets", aqlquery, collect_results_fn);

        for (auto &i : recjsonValues)
            while (std::find(i.begin(), i.end(), '"') != i.end())
                i.erase(std::find(i.begin(), i.end(), '"'));

        if (recjsonValues.size() == 0)
            return "";
        else
            return recjsonValues[0];
    }

    auto queryThermoDataSet(const std::string &idThermoDataSet) -> void
    {
        recjsonValues.clear();

        arangocpp::ArangoDBQuery aqlquery(aql_thermofun_database_from_thermodataset, arangocpp::ArangoDBQuery::AQL);
        std::string bind_value = "{\"idThermoDataSet\": \"" + idThermoDataSet + "\"}";
        aqlquery.setBindVars(bind_value);
        dbClient->selectQuery("thermodatasets", aqlquery, collect_results_fn);

        // removing NULL
        std::regex e("(?!,)[^,\n]+(?=null,)");
        auto result = std::regex_replace(recjsonValues[0], e, "");
        e = std::regex("null,");
        queriedThermoDataSet = std::regex_replace(result, e, "");
    }

    auto getDatabase(const std::string &thermodataset) -> const std::string &
    {
        std::string idThermoDataSet = idThermoDataSetFromSymbol(thermodataset);

        if (idThermoDataSet == "")
            throw std::runtime_error("Thermodataset with symbol " + thermodataset + " was not found.");
        queryThermoDataSet(idThermoDataSet);
        return queriedThermoDataSet;
    }

    auto saveDatabase(const std::string &thermodataset, const std::string &fileName) -> void
    {
        std::ofstream file(fileName);
        json j = json::parse(getDatabase(thermodataset));
        file << j.dump(2);
    }
};

DatabaseClient::DatabaseClient()
    : pimpl(new Impl())
{
}

DatabaseClient::DatabaseClient(const std::string &connection_configuration)
    : pimpl(new Impl(connection_configuration))
{
}

auto DatabaseClient::operator=(DatabaseClient other) -> DatabaseClient &
{
    pimpl = std::move(other.pimpl);
    return *this;
}

DatabaseClient::~DatabaseClient()
{
}

auto DatabaseClient::getDatabase(const std::string &thermodataset) const -> const std::string &
{
    try
    {
        return pimpl->getDatabase(thermodataset);
    }
    catch (arangocpp::arango_exception &e)
    {
        std::stringstream buffer;
        buffer << "ThermoHubClient " << e.header() << std::endl
               << e.what() << std::endl;
        throw std::runtime_error(buffer.str());
    }
    catch (json::exception &ex)
    {
        // output exception information
        std::stringstream buffer;
        buffer << "message: " << ex.what() << '\n'
               << "exception id: " << ex.id << std::endl;
        throw std::runtime_error(buffer.str());
    }
    catch (std::exception &e)
    {
        std::stringstream buffer;
        buffer << "ThermoHubClient"
               << " std::exception " << e.what() << std::endl
               << std::endl;
        throw std::runtime_error(buffer.str());
    }
    catch (...)
    {
        std::stringstream buffer;
        buffer << "ThermoHubClient"
               << " unknown exception " << std::endl;
        throw std::runtime_error(buffer.str());
    }
}

auto DatabaseClient::saveDatabase(const std::string &thermodataset, const std::string &fileName) -> void
{
    try
    {
        pimpl->saveDatabase(thermodataset, fileName);
    }
    catch (arangocpp::arango_exception &e)
    {
        std::stringstream buffer;
        buffer << "ThermoHubClient" << e.header() << std::endl
               << e.what() << std::endl;
        throw std::runtime_error(buffer.str());
    }
    catch (json::exception &ex)
    {
        // output exception information
        std::stringstream buffer;
        buffer << "message: " << ex.what() << '\n'
               << "exception id: " << ex.id << std::endl;
        throw std::runtime_error(buffer.str());
    }
    catch (std::exception &e)
    {
        std::stringstream buffer;
        buffer << "ThermoHubClient"
               << " std::exception " << e.what() << std::endl
               << std::endl;
        throw std::runtime_error(buffer.str());
    }
    catch (...)
    {
        std::stringstream buffer;
        buffer << "ThermoHubClient"
               << " unknown exception " << std::endl;
        throw std::runtime_error(buffer.str());
    }
}

} // namespace ThermoHubClient
