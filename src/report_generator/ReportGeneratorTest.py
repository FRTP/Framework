#!/usr/bin/python

import sys
import numpy as np
from ReportGenerator import generate_report
from fin_showings import get_default_functor_list


def some_rand_value(some_length):
    start_price = list(np.random.randint(low = 10, high = 200, size = 1) * 1.0 /40)[0]
    start_num_of_assets = 0
    prices = [start_price]
    assets_num = [start_num_of_assets]
    for i in range(1, some_length):
        cur_prices_step = np.random.normal(loc= 3, scale=1.0, size = 1)
        cur_assets_step = np.random.randint(low = -40, high = 80, size = 1)
        prices.append(prices[-1] + cur_prices_step)
        assets_num.append(assets_num[-1] + cur_assets_step)
    prices = np.array(prices)
    assets_num = np.array(assets_num)
    return np.stack((prices, assets_num))

def some_random_data_report():
    import datetime
    import matplotlib.dates as mdates
    numdays = 100
    base = datetime.datetime.today()
    date_list = [base - datetime.timedelta(days=x) for x in range(1, numdays)]
    date_list = np.sort(np.array(date_list))
    #print map(lambda x: str(x), date_list)
    date_list = np.array([given_date.date() for given_date in date_list])
    assets_names = ['AAPL', 'JAZHKA']
    prices_and_counts = {}
    graph_data = {}
    functors = get_default_functor_list()
    functor_names = [functor.get_name() for functor in functors]
    for asset in assets_names:
        prices_and_counts[asset] = some_rand_value(len(date_list) + 1)
        prices_and_counts_of_one_asset = prices_and_counts[asset]
        value = [(functor.apply_graph(prices_and_counts_of_one_asset, 200000),
                  functor.apply_value(prices_and_counts_of_one_asset, 200000))
                 for functor in functors]
        graph_data[asset] = value

    generate_report(x_values=date_list, path=None,
                    dict_of_lists_of_y_values=graph_data,
                    balance_history=None,
                    graph_names=functor_names)


some_random_data_report()