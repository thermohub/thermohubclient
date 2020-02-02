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

#include "DatabaseClient.h"
#include "AqlQueries.h"
#include "formulaparser/FormulaParser.h"

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
void printData(const std::string &title, const std::vector<std::string> &values)
{
    std::cout << title << std::endl;
    for (const auto &jsondata : values)
        std::cout << jsondata << std::endl;
    std::cout << std::endl;
}

struct DatabaseClient::Impl
{
    //    arangocpp::ArangoDBCollectionAPI connect{default_data};
    std::shared_ptr<arangocpp::ArangoDBCollectionAPI> dbClient;

    std::string queriedThermoDataSet;

    std::vector<std::string> recjsonValues;

    int json_indent = 2;

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
                   << " std::exception " << e.what() << std::endl;
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

    auto idThermoDataSetFromSymbol(const std::string &symbol) -> std::string
    {
        recjsonValues.clear();

        std::string query = "FOR u IN thermodatasets ";
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
        try
        {
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

    auto getDatabase(const std::string &thermodataset) -> const std::string &
    {
        std::string idThermoDataSet = idThermoDataSetFromSymbol(thermodataset);

        if (idThermoDataSet == "")
            throw std::runtime_error("Thermodataset with symbol " + thermodataset + " was not found.");
        queryThermoDataSet(idThermoDataSet);
        // select with elements - remove elements, substances and reactions
        return queriedThermoDataSet;
    }

    auto saveDatabase(const std::string &thermodataset) -> void
    {
        try
        {
            std::string fileName = thermodataset + "-thermofun.json";
            std::ofstream file(fileName);
            json j = json::parse(getDatabase(thermodataset));
            file << j.dump(json_indent);
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

    auto availableThermoDataSets() -> std::vector<std::string>
    {
        recjsonValues.clear();

        std::string query = "FOR u IN thermodatasets RETURN u.properties.symbol";
        arangocpp::ArangoDBQuery aqlquery(query, arangocpp::ArangoDBQuery::AQL);
        dbClient->selectQuery("thermodatasets", aqlquery, collect_results_fn);
        //    printData( "Select records by AQL query", recjsonValues );

        return recjsonValues;
    }

    auto testElementsInFormula(const std::string &formula, const std::vector<std::string> &elements) -> bool
    {
        FormulaParser::ChemicalFormulaParser parser;

        for (auto formelm : parser.parse(formula))
        {
            auto itr = elements.begin();
            while (itr != elements.end())
            {
                if (formelm.symbol == *itr)
                    break;
                itr++;
            }
            if (itr == elements.end())
                return false;
        }
        return true;
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

// auto DatabaseClient::getDatabase(const std::string &thermodataset, const std::vector<std::string> &elements) const -> const std::string &
// {
    // return pimpl->getDatabase(thermodataset, elements);
// }

auto DatabaseClient::saveDatabase(const std::string &thermodataset) -> void
{
    pimpl->saveDatabase(thermodataset);
}

// auto DatabaseClient::saveDatabase(const std::string &thermodataset, const std::vector<std::string> &elements) -> void
// {
    // pimpl->saveDatabase(thermodataset, elements);
// }

auto DatabaseClient::availableThermoDataSets() -> std::vector<std::string>
{
    return pimpl->availableThermoDataSets();
}

} // namespace ThermoHubClient
