import unittest
import empyrical as emp
import pyfolio as pf
import pytz
import pandas as pd
from datetime import datetime
from zipline.algorithm import TradingAlgorithm
from user_defined import initialize, handle_data, save, load, analyze

from utility import Universe, make_panels
from fin_showings import get_sharpe_value, get_instant_returns, get_cumulative_returns, upside_potential_ratio



def wrapped_init(context):
    context.panel = training_panel
    if universe.load_file != "":
        load(universe, context)
    else:
        initialize(context)
    if universe.save_file != "":
        save(universe, context)

universe = Universe()

universe.data_files = ['minute1.csv', 'minute2.csv', 'minute3.csv']
universe.frequency = 'minute'
universe.symbols = ['AAPL', 'RUB', 'FRTP']
universe.training_size = 0.5

universe.extract_parameters(print_values = False)

training_panel, test_panel = make_panels(universe)


algo = TradingAlgorithm(initialize=wrapped_init,
                        handle_data=handle_data,
                        data_frequency=universe.frequency,
                        capital_base=universe.capital_base,
                        instant_fill=True)

performance = algo.run(test_panel)

returns = pf.utils.extract_rets_pos_txn_from_zipline(performance)[0] 
sh_ratio = emp.sharpe_ratio(returns=returns, annualization=1)

#print performance.Prices
#print returns
#print get_instant_returns(performance.Prices)
#print "RIGHT       ", emp.cum_returns(returns)
#print emp.cum_returns_final(returns)
#print "NEW       ", get_cumulative_returns(returns)

class TestGetSharpeValue(unittest.TestCase):
    def test_get_sharpe_value(self):
        self.assertEqual(sh_ratio, get_sharpe_value(returns))

class TestGetInstantReturns(unittest.TestCase):
    def test_get_instant_returns(self):
        self.assertEqual(returns, get_instant_returns(performance.Prices)) 
#different representation of data

#class TestUpsidePotentialRatio(unittest.TestCase):
#    def test_upside_potential_ratio(self):
#        self.assertEqual(???, upside_potential_ratio(returns))
# is there upside potential ratio in quantopian? i haven't found

class TestGetCumulativeReturns(unittest.TestCase):
    def test_get_cumulative_returns(self):
        self.assertEqual(emp.cum_returns(returns), get_cumulative_returns(returns))
#different representation of data

#and now i don't know how to fix it

if __name__ == '__main__':
    unittest.main()