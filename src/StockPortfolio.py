#!/usr/bin/python

import numpy as np


class StockPortfolio:

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
    #   Buy stocks and get updated StockPortfolio
    #
    # @param given_names - list
    #   Names of stocks to buy. Size must be equal to given_count size.
    # @param given_count - numpy array
    #   Count of stocks to buy. Size must be equal to given_stocks size.
    # @throws Exception
    #   if len(given_names) != len(given_count)
    # @returns
    #   new instance of updated StockPortfolio if succeeded

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

    def sell(self, given_names, given_count):
        return self.buy(given_names, -given_count)

    def get_stock_count(self, stock_name):
        return self.count[self.names.index(stock_name)]

