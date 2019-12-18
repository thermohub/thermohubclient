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
#include <boost/regex.hpp>
#include <fstream>

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

        // Create database connection
        arangocpp::ArangoDBCollectionAPI connect{default_data};
    }

    Impl(const std::string &connection_configuration_file)
    {
        setfunctions();

        // Get Arangodb connection data( load settings from "examples-cfg.json" config file )
        arangocpp::ArangoDBConnection data = arangocpp::connectFromConfig(connection_configuration_file);
        // Create database connection
        arangocpp::ArangoDBCollectionAPI connect{data};
        dbClient = std::make_shared<arangocpp::ArangoDBCollectionAPI>(connect);
    }

    auto idThermoDataSetFromSymbol(const std::string &symbol) -> std::string
    {
        try
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
        catch (arangocpp::arango_exception &e)
        {
            std::cout << "TDBJsonDocument API" << e.header() << e.what() << std::endl;
        }
        catch (std::exception &e)
        {
            std::cout << "TDBJsonDocument API"
                      << " std::exception" << e.what() << std::endl;
        }
        catch (...)
        {
            std::cout << "TDBJsonDocument API"
                      << " unknown exception" << std::endl;
        }
    }

    auto queryThermoDataSet(const std::string &idThermoDataSet) -> void
    {
        try
        {
            recjsonValues.clear();

            arangocpp::ArangoDBQuery aqlquery(aql_thermofun_database_from_thermodataset, arangocpp::ArangoDBQuery::AQL);
            std::string bind_value = "{\"idThermoDataSet\": \"" + idThermoDataSet + "\"}";
            aqlquery.setBindVars(bind_value);
            dbClient->selectQuery("thermodatasets", aqlquery, collect_results_fn);

            // removing NULL
            boost::regex e("(?<=,)[^,\n]+(?=null,)");
            auto result = boost::regex_replace(recjsonValues[0], e, "");
            e = boost::regex("null,");
            queriedThermoDataSet =  boost::regex_replace(result, e, "");
        }
        catch (arangocpp::arango_exception &e)
        {
            std::cout << "TDBJsonDocument API" << e.header() << e.what() << std::endl;
        }
        catch (std::exception &e)
        {
            std::cout << "TDBJsonDocument API"
                      << " std::exception" << e.what() << std::endl;
        }
        catch (...)
        {
            std::cout << "TDBJsonDocument API"
                      << " unknown exception" << std::endl;
        }
    }

    auto getDatabase(const std::string &thermodataset) -> const std::string &
    {
        try
        {
            std::string idThermoDataSet = idThermoDataSetFromSymbol(thermodataset);
            if (idThermoDataSet == "")
                throw std::runtime_error("Thermodataset with symbol " + thermodataset + " was not found.");
            queryThermoDataSet(idThermoDataSet);
            return queriedThermoDataSet;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    auto saveDatabase(const std::string &thermodataset, const std::string &fileName) -> void
    {
        try
        {
            std::ofstream file(fileName);
            json j = json::parse(getDatabase(thermodataset));
            file << j.dump(2);
        }
        catch (json::exception &ex)
        {
            // output exception information
            std::cout << "message: " << ex.what() << '\n'
                      << "exception id: " << ex.id << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
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
    return pimpl->getDatabase(thermodataset);
}

auto DatabaseClient::saveDatabase(const std::string &thermodataset, const std::string &fileName) -> void
{
    pimpl->saveDatabase(thermodataset, fileName);
}

} // namespace ThermoHubClient
