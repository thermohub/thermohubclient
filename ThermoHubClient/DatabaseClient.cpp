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
                                                 "funrem",                                              
                                                 "ThermoFun@Remote-ThermoHub-Server",                   
                                                 "hub_main");                                           
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

    std::string resultThermoDataSet;

    std::vector<std::string> recjsonValues;

    DatabaseClientOptions options;

    int json_indent = -1;

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

        try
        {
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

    auto makeBindList(const std::vector<std::string> &list, const std::string &name, std::string &query) -> std::string
    {
        std::string bind_value = "";
        std::string bind_name = name + "List";
        if (list.size() > 0)
        {
            bind_value += ", \"" + bind_name + "\": [";
            for (auto l : list)
                if (name == "symbol")
                    bind_value += "\"" + l + "\",";
                else
                    bind_value += l + ",";
            bind_value = bind_value.substr(0, bind_value.size() - 1);
            bind_value += "]";
        }
        else
        {
            //bind_value += ", \"" + name + "\": []";
            std::string to_remove = "        FILTER s.properties." + name + " IN @" + bind_name;
            size_t nFPos = query.find(to_remove);
            while (nFPos != std::string::npos)
            {
                size_t secondNL = query.find('\n', nFPos);
                size_t firstNL = query.rfind('\n', nFPos);
                query.erase(firstNL, secondNL - firstNL);
                nFPos = query.find(to_remove);
            }
        }
        return bind_value;
    }

    auto queryThermoDataSet(const std::string &idThermoDataSet, const std::vector<std::string> &substances = {},
                            const std::vector<std::string> &classesOfSubstance = {}, const std::vector<std::string> &aggregateStates = {}) -> void
    {
        recjsonValues.clear();
        try
        {
            std::string query_ = aql_thermofun_database_from_thermodataset;
            std::string bind_value = "{\"idThermoDataSet\": \"" + idThermoDataSet + "\" ";
            bind_value += makeBindList(substances, "symbol", query_);
            bind_value += makeBindList(classesOfSubstance, "class_", query_);
            bind_value += makeBindList(aggregateStates, "aggregate_state", query_);
            bind_value += "}";

            arangocpp::ArangoDBQuery aqlquery(query_, arangocpp::ArangoDBQuery::AQL);

            std::string options = "{ \"maxPlans\" : 1, "
                              "  \"optimizer\" : { \"rules\" : [ \"-all\", \"+remove-unnecessary-filters\" ]  } } ";

            aqlquery.setBindVars(bind_value);
            aqlquery.setOptions(options);
            dbClient->selectQuery("thermodatasets", aqlquery, collect_results_fn);

            // removing NULL
            std::regex e("(?!,)[^,\n]+(?=null,)");
            auto result = std::regex_replace(recjsonValues[0], e, "");
            e = std::regex("null,");
            resultThermoDataSet = std::regex_replace(result, e, "");

            //printData( "Select records by AQL query", recjsonValues );
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

    auto removeReactionsWithReactants(json &jReactions, const std::vector<std::string> &reactants) -> void
    {
        for (auto it = jReactions.begin(); it != jReactions.end(); ++it)
        {
            for (auto it2 = it.value()["reactants"].begin(); it2 != it.value()["reactants"].end(); ++it2)
            {
                if (std::find(reactants.begin(), reactants.end(), it2.value()["symbol"]) != reactants.end())
                {
                    jReactions.erase(it--);
                    break;
                }
            }
        }
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

    auto selectDataContainingElements(const std::vector<std::string> &elems) -> void
    {
        std::vector<std::string> elements = elems;
        json jThermoDataSet = json::parse(resultThermoDataSet);
        if (elements.size() == 0)
        {
            resultThermoDataSet = jThermoDataSet.dump(json_indent);
            return;
        }

        if (!options.filterCharge) // charge is considered by default if not filtered
            elements.push_back("Zz");

        json jElements = jThermoDataSet["elements"];
        for (auto it = jElements.begin(); it != jElements.end(); ++it)
        {
            if (std::find(elements.begin(), elements.end(), it.value()["symbol"]) == elements.end())
                jElements.erase(it--);
        }

        auto jSubstances = jThermoDataSet["substances"];
        std::vector<std::string> substances;
        for (auto it = jSubstances.begin(); it != jSubstances.end(); ++it)
        {
            if (!testElementsInFormula(it.value()["formula"], elements))
            {
                substances.push_back(it.value()["symbol"]);
                jSubstances.erase(it--);
            }
        }

        auto jReactions = jThermoDataSet["reactions"];
        removeReactionsWithReactants(jReactions, substances);

        jThermoDataSet.at("elements") = jElements;
        jThermoDataSet.at("substances") = jSubstances;
        jThermoDataSet.at("reactions") = jReactions;

        resultThermoDataSet = jThermoDataSet.dump(json_indent);
    }

    auto getDatabase(const std::string &thermodataset, const std::vector<std::string> &elements,
                     const std::vector<std::string> &substances,
                     const std::vector<std::string> &classesOfSubstance,
                     const std::vector<std::string> &aggregateStates) -> const std::string &
    {
        std::string idThermoDataSet = idThermoDataSetFromSymbol(thermodataset);

        if (idThermoDataSet == "")
            throw std::runtime_error("Thermodataset with symbol " + thermodataset + " was not found.");
        queryThermoDataSet(idThermoDataSet, substances, classesOfSubstance, aggregateStates);
        selectDataContainingElements(elements);
        return resultThermoDataSet;
    }

    auto saveDatabase(const std::string &fileName) -> void
    {
        try
        {
            std::ofstream file(fileName);
            file << resultThermoDataSet;
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

    auto elementsInThermoDataSet(const std::string &thermodataset) -> std::vector<std::string>
    {
        recjsonValues.clear();

        std::string query = "FOR u IN thermodatasets \n";
        query += "FILTER u.properties.symbol == \"" + thermodataset + "\"";
        query += "FOR e, b IN 1..1 INBOUND u basis SORT e.properties.symbol RETURN e.properties.symbol";
        arangocpp::ArangoDBQuery aqlquery(query, arangocpp::ArangoDBQuery::AQL);
        dbClient->selectQuery("thermodatasets", aqlquery, collect_results_fn);
        //    printData( "Select records by AQL query", recjsonValues );

        return recjsonValues;
    }

    auto substancesInThermoDataSet(const std::string &thermodataset) -> std::vector<std::string>
    {
        recjsonValues.clear();

        std::string query = "FOR u IN thermodatasets \n";
        query += "FILTER u.properties.symbol == \"" + thermodataset + "\"";
        query += "FOR s, p IN 1..1 INBOUND u pulls SORT s.properties.symbol RETURN s.properties.symbol";
        arangocpp::ArangoDBQuery aqlquery(query, arangocpp::ArangoDBQuery::AQL);
        dbClient->selectQuery("thermodatasets", aqlquery, collect_results_fn);
        //    printData( "Select records by AQL query", recjsonValues );

        return recjsonValues;
    }

    auto reactionsInThermoDataSet(const std::string &thermodataset) -> std::vector<std::string>
    {
        recjsonValues.clear();

        std::string query = "FOR u IN thermodatasets \n";
        query += "FILTER u.properties.symbol == \"" + thermodataset + "\"";
        query += "FOR s, p IN 1..1 INBOUND u pulls ";
        query += "FOR r, t IN 1..1 OUTBOUND s takes SORT r.properties.symbol RETURN r.properties.symbol";
        arangocpp::ArangoDBQuery aqlquery(query, arangocpp::ArangoDBQuery::AQL);
        dbClient->selectQuery("thermodatasets", aqlquery, collect_results_fn);
        //    printData( "Select records by AQL query", recjsonValues );

        return recjsonValues;
    }

    auto substanceClassesInThermoDataSet(const std::string &thermodataset) -> std::vector<std::string>
    {
        recjsonValues.clear();

        std::string query = "FOR u IN thermodatasets \n";
        query += "FILTER u.properties.symbol == \"" + thermodataset + "\"";
        query += "FOR s, p IN 1..1 INBOUND u pulls RETURN DISTINCT s.properties.class_";
        arangocpp::ArangoDBQuery aqlquery(query, arangocpp::ArangoDBQuery::AQL);
        dbClient->selectQuery("thermodatasets", aqlquery, collect_results_fn);
        //    printData( "Select records by AQL query", recjsonValues );

        return recjsonValues;
    }

    auto substanceAggregateStatesInThermoDataSet(const std::string &thermodataset) -> std::vector<std::string>
    {
        recjsonValues.clear();

        std::string query = "FOR u IN thermodatasets \n";
        query += "FILTER u.properties.symbol == \"" + thermodataset + "\"";
        query += "FOR s, p IN 1..1 INBOUND u pulls RETURN DISTINCT s.properties.aggregate_state";
        arangocpp::ArangoDBQuery aqlquery(query, arangocpp::ArangoDBQuery::AQL);
        dbClient->selectQuery("thermodatasets", aqlquery, collect_results_fn);
        //    printData( "Select records by AQL query", recjsonValues );

        return recjsonValues;
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
    pimpl->json_indent = pimpl->options.json_indent_get;
    return pimpl->getDatabase(thermodataset, {}, {}, {}, {});
}

auto DatabaseClient::getDatabaseContainingElements(const std::string &thermodataset, const std::vector<std::string> &elements) const -> const std::string &
{
    pimpl->json_indent = pimpl->options.json_indent_get;
    return pimpl->getDatabase(thermodataset, elements, {}, {}, {});
}

auto DatabaseClient::getDatabaseSubset(const std::string &thermodataset, const std::vector<std::string> &elements,
                                       const std::vector<std::string> &substances,
                                       const std::vector<std::string> &classesOfSubstance,
                                       const std::vector<std::string> &aggregateStates) const -> const std::string &
{
    pimpl->json_indent = pimpl->options.json_indent_get;
    return pimpl->getDatabase(thermodataset, elements, substances, classesOfSubstance, aggregateStates);
}

auto DatabaseClient::saveDatabase(const std::string &thermodataset) -> void
{
    pimpl->json_indent = pimpl->options.json_indent_save;
    pimpl->getDatabase(thermodataset, {}, {}, {}, {});
    pimpl->saveDatabase(thermodataset + pimpl->options.databaseFileSuffix + ".json");
}

auto DatabaseClient::saveDatabaseContainingElements(const std::string &thermodataset, const std::vector<std::string> &elements) -> void
{
    pimpl->json_indent = pimpl->options.json_indent_save;
    pimpl->getDatabase(thermodataset, elements, {}, {}, {});
    pimpl->saveDatabase(thermodataset + pimpl->options.subsetFileSuffix + ".json");
}

auto DatabaseClient::saveDatabaseSubset(const std::string &thermodataset, const std::vector<std::string> &elements,
                                        const std::vector<std::string> &substances,
                                        const std::vector<std::string> &classesOfSubstance,
                                        const std::vector<std::string> &aggregateStates) -> void
{
    pimpl->json_indent = pimpl->options.json_indent_save;
    pimpl->getDatabase(thermodataset, elements, substances, classesOfSubstance, aggregateStates);
    pimpl->saveDatabase(thermodataset + pimpl->options.subsetFileSuffix + ".json");
}

auto DatabaseClient::availableThermoDataSets() -> std::vector<std::string>
{
    return pimpl->availableThermoDataSets();
}

auto DatabaseClient::elementsInThermoDataSet(const std::string &thermodataset) -> std::vector<std::string>
{
    return pimpl->elementsInThermoDataSet(thermodataset);
}

auto DatabaseClient::substancesInThermoDataSet(const std::string &thermodataset) -> std::vector<std::string>
{
    return pimpl->substancesInThermoDataSet(thermodataset);
}

auto DatabaseClient::reactionsInThermoDataSet(const std::string &thermodataset) -> std::vector<std::string>
{
    return pimpl->reactionsInThermoDataSet(thermodataset);
}

auto DatabaseClient::substanceClassesInThermoDataSet(const std::string &thermodataset) -> std::vector<std::string>
{
    return pimpl->substanceClassesInThermoDataSet(thermodataset);
}

auto DatabaseClient::substanceAggregateStatesInThermoDataSet(const std::string &thermodataset) -> std::vector<std::string>
{
    return pimpl->substanceAggregateStatesInThermoDataSet(thermodataset);
}

auto DatabaseClient::setOptions(const DatabaseClientOptions &options) -> void
{
    pimpl->options = options;
}

} // namespace ThermoHubClient
