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
    # @param historical_data <pandas DataFrame>
    #   Historical data about prices on stocks in following format:
    #    __________________________________
    #   | Date  | 'Sber' | 'Gazprom' | ... |
    #   | ------|--------|-----------|-----|
    #   | 20.02 | 32.2   | 45.7      | ... |
    #   |-------|--------|-----------|-----|
    #   | ...   | ...    | ...       | ... |
    #
    #
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
    def __init__(self, historical_data, initial_stocks_count=None,
                 current_stocks_prices=None, initial_balance=0):

        if initial_stocks_count is None:
            initial_stocks_count = []
        if current_stocks_prices is None:
            current_stocks_prices = []

        self.names = np.array(historical_data.columns)
        self.prices = current_stocks_prices
        self.current_balance = initial_balance
        self.balancing_coefficients = \
            self.compute_balancing_coefficients(historical_data)
        stocks_count = np.array(initial_stocks_count)
        self.portfolio_sequence = [StockPortfolio(stocks_count)]

    # @brief
    #   Computing balancing coefficients
    # @param all_history_prices
    #   Historical chronology of prices in format described in constructor
    #
    @staticmethod
    def compute_balancing_coefficients(historical_prices):
        return np.arange(len(historical_prices.columns))

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
    #
    def buy(self, given_names, given_count):
        new_stocks_count = self.portfolio_sequence[-1].count

        if len(given_names) != len(given_count):
            raise Exception('given_names and given_count '
                            'must have the same size')
        if not given_names:
            raise Exception("given_names mustn't be empty")

        for i in range(len(given_names)):
            current_stock_index = np.where(self.names == given_names[i])
            new_stocks_count[current_stock_index] += given_count[i]
            self.current_balance -= \
                self.prices[current_stock_index] * given_count[i]

        new_portfolio = StockPortfolio(new_stocks_count)
        self.portfolio_sequence.append(new_portfolio)

        return new_portfolio

    # @brief
    #   Sell stocks and returns updated StockPortfolio
    # @param given_names - <list>
    #   Names of stocks to buy. Size must be equal to given_count size.
    # @param given_count - <numpy array>
    #   Count of stocks to buy. Size must be equal to given_names size.
    #
    def sell(self, given_names, given_count):
        return self.buy(given_names, -given_count)

    # @brief
    #   Getting count of a particular stock we have.
    # @param stock_name
    #   The name of a stock we want.
    # @returns
    #   The amount of a particular stock we currently have.
    #
    def get_stock_count(self, stock_name):
        needed_index = np.where(self.names == stock_name)
        return self.portfolio_sequence[-1].count[needed_index]

    # @brief
    #   Updates price of the one stock
    def update_price(self, stock, new_price):
        index = np.where(self.names == stock)
        self.prices[index] = new_price

    # @brief
    #   Updates prices of given stocks
    #
    # @param stocks_names <numpy array>
    #   Array of stocks names to be updated.
    #
    # @param new_prices <numpy array>
    #   Array of new prices of the stocks to be updated.
    #
    # @throws Exception
    #   If len(stocks_names) != len(new_prices) throws an Exception.s
    #
    def update_prices(self, stocks_names, new_prices):
        if len(stocks_names) != len(new_prices):
            raise Exception("stocks_names & new_prices "
                            "must have the same size")
        for i in range(len(stocks_names)):
            self.update_price(stocks_names[i], new_prices[i])

    def get_stocks_names(self):
        return self.names

    def get_current_balance(self):
        return self.current_balance

    def get_current_portfolio(self):
        return self.portfolio_sequence[-1]
