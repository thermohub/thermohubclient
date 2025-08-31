# ThermoHubClient
A client for retrieving data from ThermoHub database.

## Live Demo
Try thermohubclient jupyter notebook demo in your browser using Binder:

[![Binder](https://mybinder.org/badge_logo.svg)](https://mybinder.org/v2/gh/thermohub/thermofun-jupyter/master?urlpath=lab/tree/thermohubclient/demo-thermohubclient.ipynb)

## Simple C++ API example

```c++
#include <iostream>
#include <ThermoHubClient/ThermoHubClient.h>
using namespace std;
using namespace ThermoHubClient;

int main()
{
    DatabaseClient dbc("hub-connection-config.json"); // database connection  (local or remote) using a configuration file

    DatabaseClient dbc_default; // default connection (remote) to db.thermohub.net

    // Save ThermoDataSet 'aq17' to a database file aq17-thermofun.json
    dbc_default.saveDatabase("aq17");
    // Save a subset of ThermoDataSet 'aq17' to a database file aq17-subset-thermofun.json, using a list of elements
    dbc_default.saveDatabaseContainingElements("aq17", {"H", "O", "Na", "K", "Si", "Al", "Cl", "Zr", "Zz"});
    // Save a subset of ThermoDataSet 'mines16' to a database file mines16-subset-thermofun.json, 
    // using a list of elements, substances, and substance classes. 
    // Only the data that fulfils all selection criteria will be saved
    dbc.saveDatabaseSubset("mines16", {"H", "O", "Na", "K", "Si", "Al", "Cl", "Zr", "Zz"},
                           {"O2@", "H2@", "NaHSiO3@", "Al(OH)2+", "NaAl(OH)4@", "Al+3", "AlH3SiO4+2", "AlOH+2", "Al(OH)4-", "H2", "O2", "H2O"},
                           {"{\"1\":\"SC_GASFLUID\"}", "{\"3\":\"SC_AQSOLVENT\"}"});

    // Get ThermoDataSet 'mines16' as a JSON string
    // all 'save...' functions have a 'get...' function counterpart
    // getDatabase, getDatabaseContainingElements, getDatabaseSubset
    std::string jsonMines16 = dbc.getDatabase("mines16");

    auto tds = dbc.availableThermoDataSets();
    cout << "ThermoDataSets" << endl;
    for (auto t : tds)
        cout << t << endl;

    cout << "Elements" << endl;
    for (auto e : dbc.elementsInThermoDataSet("aq17"))
        cout << e << " ";
    cout << endl;
    
    cout << "Substances" << endl;
    for (auto e : dbc.substancesInThermoDataSet("aq17"))
        cout << e << " ";
    cout << endl;

    cout << "Reactions" << endl;
    for (auto e : dbc.reactionsInThermoDataSet("mines16"))
        cout << e << " ";
    cout << endl;

    cout << "Substances classes" << endl;
    for (auto e : dbc.substanceClassesInThermoDataSet("aq17"))
        cout << e << " ";
    cout << endl;

    cout << "Substances aggregate state" << endl;
    for (auto e : dbc.substanceAggregateStatesInThermoDataSet("aq17"))
        cout << e << " ";
    cout << endl;

    return 0;
}
```

## Simple Python API example
```python
import thermohubclient as client

# database connection(local or remote) using a configuration file (file example in /pytests)
dbc = client.DatabaseClient("hub-connection-config.json")

# default connection(remote) to db.thermohub.net
dbc_default = client.DatabaseClient()

# Save ThermoDataSet 'aq17' to a database file aq17-thermofun.json
dbc_default.saveDatabase("aq17")
# Save a subset of ThermoDataSet 'aq17' to a database file aq17-subset-thermofun.json, using a list of elements
dbc_default.saveDatabaseContainingElements(
    "aq17", ["H", "O", "Na", "K", "Si", "Al", "Cl", "Zr", "Zz"])
# Save a subset of ThermoDataSet 'mines16' to a database file mines16-subset-thermofun.json,
# using a (optional) list of elements, substances, and substance classes.
# Only the data that fulfils all selection criteria will be saved
dbc.saveDatabaseSubset("mines16", ["H", "O", "Na", "K", "Si", "Al", "Cl", "Zr", "Zz"],
                       ["O2@", "H2@", "NaHSiO3@", "Al(OH)2+", "NaAl(OH)4@", "Al+3",
                        "AlH3SiO4+2", "AlOH+2", "Al(OH)4-", "H2", "O2", "H2O"],
                       ["{\"1\":\"SC_GASFLUID\"}", "{\"3\":\"SC_AQSOLVENT\"}"])

# Get ThermoDataSet 'mines16' as a JSON string
# all 'save...' functions have a 'get...' function counterpart
# getDatabase, getDatabaseContainingElements, getDatabaseSubset
jsonMines16 = dbc.getDatabase("mines16")

print("ThermoDataSets")
for t in dbc.availableThermoDataSets():
    print(f'{t}')
print('\n')

print("Elements")
for e in dbc.elementsInThermoDataSet("aq17"):
    print(f'{e}')
print('\n')

print("Substances")
for e in dbc.substancesInThermoDataSet("aq17"):
    print(f'{e}')
print('\n')

print("Reactions")
for e in dbc.reactionsInThermoDataSet("mines16"):
    print(f'{e}')
print('\n')

print("Substances classes")
for e in dbc.substanceClassesInThermoDataSet("aq17"):
    print(f'{e}')
print('\n')

print("Substances aggregate state")
for e in dbc.substanceAggregateStatesInThermoDataSet("aq17"):
    print(f'{e}')
print('\n')
```

## Installation using Conda

ThermoHubClient can be easily installed using [Conda](https://conda.io/docs/) package manager. If you have Conda installed, install ThermoHubClient by executing the following command:

```bash
conda install thermohubclient
```

## Build and Install ThermoHubClient from source using CMake and Conda 

This procedure uses Conda for handling all the dependencies of ThermoFun and builds ThermoFun for Windows, Mac OS X, and Linux. 

Once you have conda installed execute:

```bash
conda install -n base conda-devenv
```
This installs conda-devenv, a conda tool used to define and initialize conda environments.

Download ThermoHubClient from github

```bash
git clone https://github.com/thermohub/thermohubclient.git && cd thermohubclient 
```

In the next step we create a clean environment with all dependencies necessary to build ThermoHubClient, executing:

```bash
conda devenv 
```

In the next step we need to activate the thermofun environment 

```bash
conda activate thermohubclient
```

Build ThermoHubClient using CMake, in `/thermohubclient/

```bash
mkdir build
cd build
cmake ..
make 
make install
```

Remember to always activate thermofun environment whenever you use ThermoFun from C++ or Python. This is because conda will adjust some environment variables in your system.
