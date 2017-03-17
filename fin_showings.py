import sys
import os
import numpy as np


_epsilon_ = 1e-20


sys.path.append(os.path.dirname(os.path.abspath(__file__)) + '/src')
from ReportGenerator import generate_report


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

def make_functor_when_returns_not_given(function_name, function_method_on_returns):
    return make_functor_from_function(function_name, lambda x: function_method_on_returns(get_instant_returns(x)))


def get_sharpe_value(ar):
    if len(ar) == 1:
        return np.mean(ar) / (np.std(ar) + _epsilon_)
    else:
        return np.mean(ar) / (np.std(ar, ddof=1) + _epsilon_)


def get_instant_returns(data):
    prices = np.array(data).T[0]
    assets_number = np.array(data).T[1]
    shift_prices = np.array(prices[:-1])
    our_prices = np.array(prices[1:])
    assets_number_shifted = np.array(assets_number[1:])
    return (our_prices - shift_prices) * assets_number_shifted


def get_cumulative_returns(instant_returns):
    return np.cumsum(instant_returns)


def upside_potential_ratio(returns):
    nonnegative_returns = np.where(returns >= 0, returns, 0)
    nonpositive_returns = np.where(returns <= 0, returns, 0)
    return np.mean(nonnegative_returns) / (np.std(nonpositive_returns) + _epsilon_)


FunctorsList = [make_functor_from_function('sharpe_ratio', get_sharpe_value)(),
                make_functor_from_function('cumulative_returns', get_cumulative_returns)(),
                make_functor_from_function('upside_potential', upside_potential_ratio)()
                ]

generate_report(get_instant_returns([[2, 5], [3, 8], [1, 4]]), FunctorsList, "report.pdf")

