import thermohubclient as client
import pytest as pytest
import unittest


class TestDatabaseClient(unittest.TestCase):

    def setUp(self):
        self.dbc = client.DatabaseClient("pytests/hub-connection-config.json") # using config file
        self.dbc2 = client.DatabaseClient() # using default connection
    
    def test_get_database_from_hub(self):
        elements = self.dbc.elementsInThermoDataSet("aq17")
        substances = self.dbc.substancesInThermoDataSet('aq17')
        classes = self.dbc2.substanceClassesInThermoDataSet('aq17') 
        aggregatestates = self.dbc2.substanceAggregateStatesInThermoDataSet('aq17') 
        assert len(elements) == 11
        assert elements[6] == '"Mg"'
        assert len(substances) == 161
        assert substances[160] == '"Zoisite"'
        assert len(classes) == 4
        # assert classes[2] == '{"1":"SC_GASFLUID"}'
        assert len(aggregatestates) == 3
        # assert aggregatestates[1] == '{"4":"AS_AQUEOUS"}'

    def test_save_database(self):
        self.dbc.saveDatabase('aq17')
        self.dbc.saveDatabaseSubset('aq17', elements=["Al", "Si", "O", "Zz"])

