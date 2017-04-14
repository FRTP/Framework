import unittest
import empyrical as emp
import pyfolio as pf
import pandas as pd
from zipline.algorithm import TradingAlgorithm
from user_defined import initialize, handle_data, save, load, analyze
import numpy as np

from utility import Universe, make_panels
from fin_showings import get_sharpe_value,\
    get_cumulative_returns, get_strange_returns
from pandas.util.testing import assert_frame_equal


def wrapped_init(context):
    context.panel = training_panel
    if universe.load_file != "":
        load(universe, context)
    else:
        initialize(context)
    if universe.save_file != "":
        save(universe, context)


our_dataframe = pd.read_csv('daily1.csv', index_col=0, parse_dates=True).values
to_extract = int(0.8*our_dataframe.shape[0])
our_dataframe = our_dataframe[(to_extract - 1):, :]
initial_price = our_dataframe[0][-1]

universe = Universe()

universe.data_files = ['daily1.csv']
universe.frequency = 'minute'
universe.symbols = ['AAPL']
universe.training_size = to_extract

universe.extract_parameters(print_values=False)

training_panel, test_panel = make_panels(universe)

algo = TradingAlgorithm(initialize=wrapped_init,
                        handle_data=handle_data,
                        data_frequency=universe.frequency,
                        capital_base=universe.capital_base,
                        instant_fill=True)
performance = algo.run(test_panel)
our_trade_data = analyze(results=performance, universe=universe)
our_trade_data = np.vstack((np.array([initial_price, 0]), our_trade_data))
returns = pf.utils.extract_rets_pos_txn_from_zipline(performance)[0]
sh_ratio = emp.sharpe_ratio(returns=returns, annualization=1)


class TestGetSharpeValue(unittest.TestCase):
    def test_get_sharpe_value(self):
        self.assertLessEqual(abs(sh_ratio - get_sharpe_value(returns)), 1e-10)


class TestGetInstantReturns(unittest.TestCase):
    def test_get_instant_returns(self):
        np.testing.assert_array_almost_equal(get_strange_returns
                                             (our_trade_data,
                                              universe.capital_base),
                                             returns, 3)


class TestGetCumulativeReturns(unittest.TestCase):
    def test_get_cumulative_returns(self):
        assert_frame_equal(pd.DataFrame(emp.cum_returns(returns)),
                           pd.DataFrame(get_cumulative_returns(returns)))


if __name__ == '__main__':
    unittest.main()
