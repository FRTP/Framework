#!/usr/bin/python

import numpy as np


# Stock portfolio.
# @brief
#   Provides the interface to perform stocks manipulations.


class StockPortfolio:

    # @param stocks_names - <list>
    #   All stocks that considered by portfolio.
    #
    # @param stocks_count - <numpy array>
    #   The number of each stock we currently have.
    #
    # @param stocks_prices - <numpy array>
    #   The current price of each stock.
    #
    # @param balance - <float>
    #   Amount of money we currently have ( may be negative ).
    #
    # @var coefficients ... todo
    #   ...

    def __init__(self, stocks_names, stocks_count, stocks_prices, balance):
        self.names = stocks_names
        self.count = np.array(stocks_count)
        self.prices = stocks_prices
        self.coefficients = self.gen_coefs()
        self.current_balance = balance

    def gen_coefs(self):
        # todo : gen somehow
        raise NotImplementedError

    # Buy stocks
    # @brief
    #   Buy stocks and get updated StockPortfolio.
    #
    # @param given_names - list
    #   Names of stocks to buy. Size must be equal to given_count size.
    # @param given_count - numpy array
    #   Count of stocks to buy. Size must be equal to given_names size.
    # @throws Exception
    #   if len(given_names) != len(given_count)
    # @returns
    #   New instance of updated StockPortfolio if succeeded.

    def buy(self, given_names, given_count):
        new_count = self.count
        new_balance = self.current_balance

        if len(given_names) != len(given_count):
            raise Exception('given_names and given_count must have the same size')

        for i in range(len(given_names)):
            index = self.names.index(given_names[i])
            new_count[index] += 1
            new_balance -= self.prices[index] * given_count[i]

        return StockPortfolio(stocks_names=self.names,
                              stocks_count=new_count,
                              stocks_prices=self.prices,
                              balance=new_balance)

    # @brief
    #   Sell stocks and get updated StockPortfolio
    # @param given_names - <list>
    #   Names of stocks to buy. Size must be equal to given_count size.
    # @param given_count - <numpy array>
    #   Count of stocks to buy. Size must be equal to given_names size.

    def sell(self, given_names, given_count):
        return self.buy(given_names, -given_count)

    # @brief
    #   Getting count of a particular stock we have.
    # @param stock_name
    #   The name of a stock we want.
    # @returns
    #   The amount of a particular stock we currently have.

    def get_stock_count(self, stock_name):
        return self.count[self.names.index(stock_name)]

