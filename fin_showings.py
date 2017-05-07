import sys
import os
import numpy as np

_epsilon_ = 1e-20

sys.path.append(os.path.dirname(os.path.abspath(__file__)) + '/src')


def make_functor_from_function(function_name, function_graph_method = None, function_value_method = None):
    class SomeFunctor:
        def __init__(self):
            self.name = function_name
            self.function_graph_method = function_graph_method
            self.function_value_method = function_value_method

        def apply_graph(self, *args):
            if function_graph_method is not None:
                return self.function_graph_method(*args)
            else:
                return None
        def apply_value(self, *args):
            if function_value_method is not None:
                return self.function_value_method(*args)
            else:
                return None

        def get_name(self):
            return self.name

    return SomeFunctor


def make_function_when_returns_not_given(function_method_on_returns):
    return (lambda x, y: function_method_on_returns(get_instant_returns(x, y)))


def get_sharpe_value(ar):
    if len(ar) == 1:
        return np.mean(ar) / (np.std(ar) + _epsilon_)
    else:
        return np.mean(ar) / (np.std(ar, ddof=1) + _epsilon_)

def get_sharpe_value_array_like(ar):
    #print ar.shape, 'kuku'
    return np.array([get_sharpe_value(ar[:i]) for i in np.arange(ar.shape[0]) + 1])

def get_instant_returns_helper(data):
    prices = np.array(data)[0]
    assets_number = np.array(data)[1]
    shift_prices = np.array(prices[:-1])
    our_prices = np.array(prices[1:])
    assets_number_shifted = np.array(assets_number[1:])
    return (our_prices - shift_prices) * assets_number_shifted


def get_instant_returns(data, start_cash):
    #print get_instant_returns_helper(data)
    our_cash = get_instant_returns_helper(data) + start_cash
    help_ar = np.hstack((np.array([start_cash]), our_cash[:-1]))
    return our_cash / help_ar - 1


def get_strange_returns(data, start_cash):
    our_cash = get_instant_returns_helper(data) + start_cash
    return our_cash / start_cash - 1


def get_cumulative_returns_ar_like(instant_returns):
    return np.cumprod(instant_returns + 1) - 1

def get_cum_returns_final(instant_returns):
    return get_cumulative_returns_ar_like(instant_returns)[-1]


def upside_potential_ratio(returns):
    nonnegative_returns = np.where(returns >= 0, returns, 0)
    nonpositive_returns = np.where(returns <= 0, returns, 0)
    return np.mean(nonnegative_returns) \
        / (np.std(nonpositive_returns) + _epsilon_)


def get_upside_potential_array_like(ar):
    return np.array([upside_potential_ratio(ar[:i]) for i in np.arange(ar.shape[0]) + 1])

def get_default_functor_list():
    FunctorsList = [make_functor_from_function('sharpe_ratio',
                                           make_function_when_returns_not_given(get_sharpe_value_array_like),
                                           make_function_when_returns_not_given(get_sharpe_value))(),
                make_functor_from_function('cumulative_returns',
                                           make_function_when_returns_not_given(get_cumulative_returns_ar_like),
                                           make_function_when_returns_not_given(get_cum_returns_final))(),
                make_functor_from_function('upside_potential',
                                           make_function_when_returns_not_given(get_upside_potential_array_like),
                                           make_function_when_returns_not_given(upside_potential_ratio))()
                ]
    return FunctorsList