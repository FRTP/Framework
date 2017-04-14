import sys
import os
import numpy as np

_epsilon_ = 1e-20

sys.path.append(os.path.dirname(os.path.abspath(__file__)) + '/src')


def make_functor_from_function(function_name, function_method):
    class SomeFunctor:
        def __init__(self):
            self.name = function_name
            self.function = function_method

        def apply(self, data_array):
            return self.function(data_array)

        def get_name(self):
            return self.name

    return SomeFunctor


def make_functor_when_returns_not_given(function_name,
                                        function_method_on_returns):
    return make_functor_from_function(
        function_name,
        lambda x: function_method_on_returns(get_instant_returns(x)))


def get_sharpe_value(ar):
    if len(ar) == 1:
        return np.mean(ar) / (np.std(ar) + _epsilon_)
    else:
        return np.mean(ar) / (np.std(ar, ddof=1) + _epsilon_)


def get_instant_returns_helper(data):
    prices = np.array(data).T[0]
    assets_number = np.array(data).T[1]
    shift_prices = np.array(prices[:-1])
    our_prices = np.array(prices[1:])
    assets_number_shifted = np.array(assets_number[1:])
    return (our_prices - shift_prices) * assets_number_shifted


def get_instant_returns(data, start_cash):
    our_cash = get_instant_returns_helper(data) + start_cash
    help_ar = np.hstack((np.array([start_cash]), our_cash[:-1]))
    return our_cash / help_ar - 1


def get_strange_returns(data, start_cash):
    our_cash = get_instant_returns_helper(data) + start_cash
    return our_cash / start_cash - 1


def get_cumulative_returns(instant_returns):
    return np.cumprod(instant_returns + 1) - 1


def upside_potential_ratio(returns):
    nonnegative_returns = np.where(returns >= 0, returns, 0)
    nonpositive_returns = np.where(returns <= 0, returns, 0)
    return np.mean(nonnegative_returns) \
        / (np.std(nonpositive_returns) + _epsilon_)


FunctorsList = [make_functor_from_function('sharpe_ratio',
                                           get_sharpe_value)(),
                make_functor_from_function('cumulative_returns',
                                           get_cumulative_returns)(),
                make_functor_from_function('upside_potential',
                                           upside_potential_ratio)()
                ]
