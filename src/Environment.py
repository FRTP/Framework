#!/usr/bin/python

from datetime import datetime
import numpy as np


class StockPortfolio:
    # @throws ValueError
    #   If current_date has wrong format (not StockPortfolio.TIME_FORMAT)
    #
    # @throws TypeError
    #   If current_date is not <str> or <datetime>
    #
    def __init__(self, stock_count, current_date):
        self.count = stock_count
        self.date = StockPortfolio.to_datetime(current_date)

    # @brief
    # @param all_stocks_names <np array of 'str'>
    # @param stock_name
    def get_stock_count(self, all_stocks_names, stock_name):
        index = all_stocks_names.tolist().index(stock_name)
        return self.count[index]

    TIME_FORMAT = "%Y-%m-%dT%H:%M:%S"
    ERR_MSG_WRONG_TIME_FORMAT = "Wrong input format. Needed following:" + \
                                TIME_FORMAT
    ERR_MSG_WRONG_TIME_TYPE = "Wrong input. Needed 'str' or 'datetime'"

    @staticmethod
    def to_datetime(given_date):
        if type(given_date) == str:
            try:
                given_date = datetime.strptime(given_date,
                                               StockPortfolio.TIME_FORMAT)
            except ValueError:
                raise ValueError(StockPortfolio.ERR_MSG_WRONG_TIME_FORMAT)

        elif type(given_date) != datetime:
            raise TypeError(StockPortfolio.ERR_MSG_WRONG_TIME_TYPE)

        return given_date


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
    #    ___________________________________________________
    #   | Date & Time            | 'Sber' | 'Gazprom' | ... |
    #   | -----------------------|--------|-----------|-----|
    #   | 1994-11-05T13:15:30    | 32.2   | 45.7      | ... |
    #   |------------------------|--------|-----------|-----|
    #   | %Y-%m-%dT%H:%M:%S      | ...    | ...       | ... |
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
    def __init__(self, hist_data, initial_stocks_count=None,
                 current_stocks_prices=None, initial_date=None,
                 initial_balance=0):

        if initial_stocks_count is None:
            initial_stocks_count = []
        if current_stocks_prices is None:
            current_stocks_prices = []
        if initial_date is None:
            initial_date = ""

        self.names = np.array(hist_data.columns)[1:]
        self.prices = current_stocks_prices
        self.current_balance = initial_balance
        self.balancing_coefficients = \
            self.compute_balancing_coefficients(hist_data)
        stocks_count = np.array(initial_stocks_count)
        self.portfolio_sequence = [StockPortfolio(stocks_count, initial_date)]

    # @brief
    #   Computing balancing coefficients
    #
    # @param all_history_prices
    #   Historical chronology of prices in format described in constructor
    #
    @staticmethod
    def compute_balancing_coefficients(historical_prices):
        without_dates = np.array(historical_prices)[:, 1:]
        means = without_dates.mean(axis=0)
        max_mean = np.max(means)
        coefficients = [round(float(max_mean) / float(cur_mean))
                        for cur_mean in means]

        return np.array(coefficients)

    # Buy stocks
    # @brief
    #   Buy stocks and get updated StockPortfolio.
    #
    # @param given_names - <list>
    #   Names of stocks to buy. Size must be equal to given_count size.
    #
    # @param given_count - <numpy array>
    #   Count of stocks to buy. Size must be equal to given_names size.
    #
    # @param purchase_date <datetime>
    #   Date of the purchase
    #
    # @throws Exception
    #   if len(given_names) != len(given_count)
    #
    # @returns
    #   New instance of updated StockPortfolio if succeeded.
    #
    def buy(self, given_names, given_count, purchase_date):
        new_stocks_count = self.portfolio_sequence[-1].count

        if len(given_names) != len(given_count):
            raise Exception('given_names and given_count '
                            'must have the same size')
        if not given_names:
            raise Exception("given_names mustn't be empty")

        for i in range(len(given_names)):
            current_stock_idx = self.names.tolist().index(given_names[i])
            current_coef = self.balancing_coefficients[current_stock_idx]
            new_stocks_count[current_stock_idx] += \
                given_count[i] * current_coef
            self.current_balance -= \
                self.prices[current_stock_idx] * given_count[i] * current_coef

        new_portfolio = StockPortfolio(new_stocks_count, purchase_date)
        self.portfolio_sequence.append(new_portfolio)

        return new_portfolio

    # @brief
    #   Sell stocks and returns updated StockPortfolio
    #
    # @param given_names - <list>
    #   Names of stocks to buy. Size must be equal to given_count size.
    #
    # @param given_count - <numpy array>
    #   Count of stocks to buy. Size must be equal to given_names size.
    #
    def sell(self, given_names, given_count, purchase_date):
        return self.buy(given_names, -given_count, purchase_date)

    # @brief
    #   Getting count of a particular stock we have.
    #
    # @param stock_name
    #   The name of a stock we want.
    #
    # @returns
    #   The amount of a particular stock we currently have.
    #
    def get_stock_count(self, stock_name):
        needed_index = self.names.tolist().index(stock_name)
        return self.portfolio_sequence[-1].count[needed_index]

    # @brief
    #   Updates price of the one stock
    def update_price(self, stock, new_price):
        index = self.names.tolist().index(stock)
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

    # @brief
    #   Extracts needed_stock's price at needed_date
    #
    # @param needed_stock <str>
    #   Needed stock name.
    #
    # @param needed_date <str> in following format :%Y-%m-%dT%H:%M:%S
    #    or <datetime>.
    #
    # @throws ValueError
    #   If needed_date has wrong format (not StockPortfolio.TIME_FORMAT)
    #
    # @throws TypeError
    #   If needed_date is not <str> or <datetime>
    #
    # @returns
    #   Needed stock price - if found.
    #   None - if wasn't found.
    #
    @staticmethod
    def get_price_by_date(hist_data, needed_stock, needed_date):
        needed_date = StockPortfolio.to_datetime(needed_date)
        idx = Environment.__find__row__by__date__(hist_data, needed_date)

        if idx is not None:
            return hist_data[needed_stock][idx]
        else:
            return None

    @staticmethod
    def __find__row__by__date__(hist_data, given_date):
        if type(given_date) != datetime:
            raise TypeError("given_date must be <datetime> type.")

        for i in range(len(hist_data)):
            cur_date = datetime.strptime(hist_data[:, 0][i],
                                         StockPortfolio.TIME_FORMAT)
            if cur_date == given_date:
                return i
        return None

    @staticmethod
    def get_train_period(hist_data, start_date=None, length=0, ratio=0.):
        if length == 0 and ratio == 0:
            return None

        if start_date is None:
            first_idx = 0
        else:
            start_date = StockPortfolio.to_datetime(start_date)
            first_idx = Environment.__find__row__by__date__(hist_data,
                                                            start_date)
            if first_idx is None:
                raise ValueError("No such start_date in hist_data.")

        if length == 0:
            length = len(hist_data) * ratio
        last_idx = min(len(hist_data), first_idx + length)

        return hist_data[first_idx:last_idx]

    def get_stocks_names(self):
        return self.names

    def get_current_balance(self):
        return self.current_balance

    def get_current_portfolio(self):
        return self.portfolio_sequence[-1]
