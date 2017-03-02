#!/usr/bin/python

import numpy as np


class StockPortfolio:

    def __init__(self, stock_count):
        self.count = stock_count


    def get_stock_count(self, all_stocks_names, stock_name):
        index = all_stocks_names.index(stock_name)
        return self.count[index]

# Environment.
# @brief
#   Provides the interface to perform stocks manipulations.


class Environment:

    # VARIABLES
    #
    # @var names
    #   All stocks names we consider
    #
    # @var prices
    #   Current prices of stocks we consider.
    #
    # @var current_balance
    #
    # @var balancing_coefficients
    #
    # @var current_balance
    #   Amount of money we currently have.
    #
    # @var portfolio_sequence <StockPortfolio list>
    #   Our trading history.

    # @params
    #
    # @param historical_data
    #   Historical data about prices on stocks in following format:
    #   [(name, historical_prices, ( ... ), ... ], where :
    #   - name is stock name
    #   - historical_prices is np.array of history of cost of the stock.
    #
    # @param initial_stocks_count - <numpy array>
    #   The number of each stock we currently have.
    #
    # @param current_stocks_prices - <numpy array>
    #   The current price of each stock.
    #
    # @param initial_balance - <float>
    #   Amount of money we currently have ( may be negative ).
    #
    def __init__(self, historical_data, initial_stocks_count,
                 current_stocks_prices, initial_balance):
        self.names = [name for (name, _) in historical_data]
        self.prices = current_stocks_prices
        self.current_balance = initial_balance
        self.balancing_coefficients = \
            self.compute_balancing_coefficients(historical_data)
        stocks_count = np.array(initial_stocks_count)
        self.portfolio_sequence = [StockPortfolio(stocks_count)]

    # @brief
    #   Computing balancing coefficients
    # @param all_history_prices
    #   Historical chronology of prices in such format:
    #   [ (stock_name, np.array(prices history)), ( ... ), ... ]

    @staticmethod
    def compute_balancing_coefficients(historical_prices):
        means = np.array([prices.mean() for (_, prices) in historical_prices])
        max_mean = np.max(means)
        return means/float(max_mean)

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
        new_stocks_count = self.portfolio_sequence[-1].count

        if len(given_names) != len(given_count):
            raise Exception('given_names and given_count '
                            'must have the same size')
        if not given_names:
            raise Exception("given_names mustn't be empty")

        for i in range(len(given_names)):
            current_stock_index = self.names.index(given_names[i])
            new_stocks_count[current_stock_index] += given_count[i]
            self.current_balance -= self.prices[current_stock_index] * given_count[i]

        new_portfolio = StockPortfolio(new_stocks_count)
        self.portfolio_sequence.append(new_portfolio)

        return new_portfolio

    # @brief
    #   Sell stocks and returns updated StockPortfolio
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
        needed_index = self.names.index(stock_name)
        return self.portfolio_sequence[-1].count[needed_index]

    def get_stocks_names(self):
        return self.names

    def get_current_balance(self):
        return self.current_balance

    def get_current_portfolio(self):
        return self.portfolio_sequence[-1]
