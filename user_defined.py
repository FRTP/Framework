from zipline.api import order, record, symbol,\
                        set_commission, set_slippage
from pandas import to_pickle, read_pickle
import numpy as np
from zipline.finance.commission import PerDollar, PerShare, PerTrade
from zipline.finance.slippage import VolumeShareSlippage, FixedSlippage


# Define algorithm
def initialize(context):
    set_slippage(VolumeShareSlippage(volume_limit=1.0, price_impact=0.0))
    set_slippage(FixedSlippage(spread=0.0))
    # Here we can use data from csv and write learned parameters into context
    context.panel  # Panel with training data.
    context.result = 12  # Here we can save result for serialization.
    set_commission(PerShare(cost=0, min_trade_cost=0))
    set_commission(PerTrade(cost=0))
    set_commission(PerDollar(cost=0))


def handle_data(context, data):
    context.panel  # Here we have access to training data also.
    # Make solution using the result of learning:
    stock_name = context.panel.axes[0][0]
    if not int(data[symbol(stock_name)].price) % context.result:
        order(symbol(stock_name), 10)
    # Record some values for analysis in 'analyze()'
    cur_sid = context.panel.axes[0].values[0]
    record(Prices=data[symbol(cur_sid)].price)
    record(Number=context.portfolio.positions[symbol(cur_sid)].amount)


def load(universe, context):
    context.result = read_pickle(universe.load_file)


def save(universe, context):
    to_pickle(context.result, universe.save_file)


def analyze(results, universe):
    return np.vstack((np.array(results.Prices), np.array(results.Number))).T
