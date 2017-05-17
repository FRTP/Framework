from datetime import datetime
from report_generator.ReportGenerator import generate_report
import numpy as np
import pandas as pd


class AssetsPortfolio:
    # @throws ValueError
    #   If current_date has wrong format (not AssetsPortfolio.TIME_FORMAT)
    #
    # @throws TypeError
    #   If current_date is not <str> or <datetime>
    #
    def __init__(self, assets_count, current_date):
        self.count = assets_count
        self.date = AssetsPortfolio.to_datetime(current_date)

    # @brief
    # @param all_assets_names <np array of 'str'>
    # @param asset_name
    def get_asset_count(self, all_assets_names, asset_name):
        index = all_assets_names.index(asset_name)
        return self.count[index]

    TIME_FORMAT = "%Y-%m-%d"
    ERR_MSG_WRONG_TIME_FORMAT = "Wrong input format. Needed following:" + \
                                TIME_FORMAT
    ERR_MSG_WRONG_TIME_TYPE = "Wrong input. Needed 'str' or 'datetime'"

    @staticmethod
    def to_datetime(given_date):
        if type(given_date) == str:
            try:
                given_date = datetime.strptime(given_date,
                                               AssetsPortfolio.TIME_FORMAT)
            except ValueError:
                raise ValueError(AssetsPortfolio.ERR_MSG_WRONG_TIME_FORMAT)

        elif type(given_date) != datetime:
            raise TypeError(AssetsPortfolio.ERR_MSG_WRONG_TIME_TYPE)

        return given_date


# Environment.
# @brief
#   Provides the interface to perform assets manipulations.


class Environment:
    unconstrained_volume = 1e20

    # VARIABLES
    #
    # @var names
    #   All assets names we consider
    #
    # @var prices
    #   Current prices of assets we consider.
    #
    # @var current_balance
    #
    # @var balancing_coefficients
    #
    # @var current_balance
    #   Amount of money we currently have.
    #
    # @var portfolio_sequence <AssetsPortfolio list>
    #   Our trading history.

    # @params
    #
    # @param historical_data list <pandas DataFrame>
    #   Historical data is list of pd_frames
    #   with information on assets. Each frame in following format:
    ##
    # LIST PD.DATA_FRAME:
    #  __                                                            __
    # |                                                                |
    # |  ____________________________________________________________  |
    # | | Date       | Open | High | Low | Close | Volume | Adj close| |
    # | | -----------|------|------|-----|-------|--------|----------| |
    # | | 2000-02-18 | 32.2 | 45.7 | ... |  ...  |  ....  |   ...    | |
    # | |------------|------|------|-----|-------|--------|----------| |
    # | | %Y-%m-%d   | ...  | ...  | ... |  ...  |  ....  |   ...    | |
    # |                                                                |
    # |                                                                |
    #  --                                                            --
    #
    # @param initial_assets_count - <numpy array>
    #   The number of each asset we currently have.
    #
    # @param current_assets_prices - <numpy array>
    #   The current price of each asset.
    #
    # @param initial_balance - <float>
    #   Amount of money we currently have ( may be negative ).
    #
    def __init__(self, hist_data, names_list, current_assets_prices=None,
                 initial_assets_count=None, initial_date=None,
                 initial_balance=0, history_limit=np.inf, initial_cash=None,
                 commission_coefficient=0.006):

        if len(hist_data) != len(names_list):
            raise ValueError("hist data and names_list "
                             "must have the same sizes")

        if initial_assets_count is None:
            initial_assets_count = np.zeros(len(hist_data))
        if current_assets_prices is None:
            current_assets_prices = np.zeros(len(hist_data))
        if initial_date is None:
            initial_date = datetime.now()
        if hist_data is None:
            hist_data = [None] * len(names_list)
        if initial_cash is None:
            initial_cash = 100000

        self.history_data = hist_data
        self.assets_names = names_list
        self.prices = current_assets_prices
        self.current_balance = initial_balance
        self.balancing_coefficients = \
            self.compute_balancing_coefficients(hist_data)
        assets_count = np.array(initial_assets_count)
        self.portfolio_sequence = [AssetsPortfolio(assets_count, initial_date)]
        self.history_limit = history_limit
        self.balance_history = [initial_balance]
        self.init_cash = initial_cash
        self.commission_coefficient = commission_coefficient

    # @brief
    #   Computing balancing coefficients
    #
    # @param all_history_prices
    #   Historical chronology of prices in format described in constructor
    #
    def compute_balancing_coefficients(self, hist_data_list):

        #    without_dates = np.array(historical_prices)[:, 1:]
        #    means = without_dates.mean(axis=0)
        #    max_mean = np.max(means)
        #    coefficients = [round(float(max_mean) / float(cur_mean))
        #                    for cur_mean in means]
        #
        #    return np.array(coefficients)

        return np.ones(len(self.assets_names))

    # Buy assets
    # @brief
    #   Buy assets and get updated AssetsPortfolio.
    #
    # @param given_names - <list>
    #   Names of assets to buy. Size must be equal to given_count size.
    #
    # @param given_count - <numpy array>
    #   Count of assets to buy. Size must be equal to given_names size.
    #
    # @param purchase_date <datetime>
    #   Date of the purchase. Needed for the AssetsPortfolio constructor.
    #   Cause portfolio must have info about date.
    #
    # @throws Exception
    #   if len(given_names) != len(given_count)
    #
    # @returns
    #   New instance of updated AssetsPortfolio if succeeded.
    #
    def buy(self, given_names, given_count, purchase_date):
        new_assets_count = self.portfolio_sequence[-1].count

        if len(given_names) != len(given_count):
            raise Exception('given_names and given_count '
                            'must have the same size')
        if not given_names:
            raise Exception("given_names mustn't be empty")

        for i in range(len(given_names)):
            current_asset_idx = self.assets_names.index(given_names[i])
            current_coef = self.balancing_coefficients[current_asset_idx]
            new_assets_count[current_asset_idx] += \
                given_count[i] * current_coef
            purchase_prices = self.prices[current_asset_idx] *\
                given_count[i] * current_coef
            purchase_comissions = np.abs(purchase_prices) * \
                self.commission_coefficient
            self.current_balance -= \
                (self.prices[current_asset_idx] * given_count[i] *
                 current_coef + purchase_comissions)
            self.balance_history.append(self.current_balance)

        new_portfolio = AssetsPortfolio(new_assets_count, purchase_date)
        self.portfolio_sequence.append(new_portfolio)
        # self.update_prices(given_names, self.prices[np.array([
        # self.get_asset_index(asset_name) for asset_name in given_names])],
        #  purchase_date)

        return new_portfolio

    # @brief
    #   Sell assets and returns updated AssetsPortfolio
    #
    # @param given_names - <list>
    #   Names of assets to buy. Size must be equal to given_count size.
    #
    # @param given_count - <numpy array>
    #   Count of assets to buy. Size must be equal to given_names size.
    #
    def sell(self, given_names, given_count, purchase_date):
        return self.buy(given_names, -given_count, purchase_date)

    # @brief
    #   Getting count of a particular asset we have.
    #
    # @param asset_name
    #   The name of a asset we want.
    #
    # @returns
    #   The amount of a particular asset we currently have.
    #
    def get_asset_count(self, asset_name):
        needed_index = self.assets_names.index(asset_name)
        return self.portfolio_sequence[-1].count[needed_index]

    # @brief
    #   Updates price of the one asset
    def update_price(self, asset, new_price):
        index = self.assets_names.index(asset)
        self.prices[index] = new_price

    # @brief
    #   Updates prices of given assets
    #
    # @param assets_names <numpy array>
    #   Array of assets names to be updated.
    #
    # @param new_prices <numpy array>
    #   Array of new prices of the assets to be updated.
    #
    # @throws Exception
    #   If len(assets_names) != len(new_prices) throws an Exception.s
    #
    def update_prices(self, assets_names, new_prices, current_date):
        if len(assets_names) != len(new_prices):
            raise Exception("assets_names & new_prices "
                            "must have the same size")
        for i in range(len(assets_names)):
            self.update_price(assets_names[i], new_prices[i])
            self.history_data[i] = Environment \
                .add_historical_data(self.history_data[i],
                                     new_prices[i], current_date)

    @staticmethod
    def add_historical_data(history_dataframe, current_price, current_date):
        current_date = AssetsPortfolio.to_datetime(current_date)
        to_add = [current_date] + [current_price] * 4 + \
                 [Environment.unconstrained_volume] + [current_price]
        to_add = np.array(to_add)
        to_add = pd.DataFrame(to_add, columns=np.array(['Date',
                                                        'Open',
                                                        'High',
                                                        'Low',
                                                        'Close',
                                                        'Volume',
                                                        'Adj close']))
        if history_dataframe is None:
            return to_add
        return history_dataframe.append(to_add, ignore_index=True)

    def get_prices_by_date(self, asset_name=None, date=None):
        if date is None or asset_name is None:
            return None
        if type(date) != str:
            raise TypeError("date must be string : "
                            + AssetsPortfolio.TIME_FORMAT)
        asset_index = self.get_asset_index(asset_name)
        asset_df = self.history_data[asset_index]

        df_row = asset_df.loc[asset_df['Date'] == date]

        if len(df_row) == 0:
            raise ValueError("No such entry in the history of " + asset_name)

        return df_row

    # Gives rows in the asset_hist_data
    #   which are appropriate to the start_date and length of the period.
    # def get_train_period(self, asset_name, start_date=None, length=0,
    # ratio=0.):
    #     if length == 0 and ratio == 0:
    #         return None
    #
    #     if start_date is None:
    #         first_idx = 0
    #     else:
    #         start_date = AssetsPortfolio.to_datetime(start_date)
    #         first_idx = Environment.__find__row__by__date__(asset_hist_data,
    #                                                         start_date)
    #         if first_idx is None:
    #             raise ValueError("No such start_date in asset_hist_data.")
    #
    #     if length == 0:
    #         length = len(asset_hist_data) * ratio
    #     last_idx = min(len(asset_hist_data), first_idx + length)
    #
    #     return asset_hist_data[first_idx:last_idx]

    # Just gives names of considered assets.
    def get_assets_names(self):
        return self.assets_names

    def get_asset_index(self, asset_name=None):
        if asset_name is None:
            return None
        else:
            return self.assets_names.index(asset_name)

    # Environments maintains current balance
    #   and can be asked for it at any moment.
    def get_current_balance(self):
        return self.current_balance

    # Gives the last portfolio in the portfolio chain (sequence).
    def get_current_portfolio(self):
        return self.portfolio_sequence[-1]

    def generate_report(self, functors, path=None):
        assets_count = len(self.assets_names)
        portfolio_count = len(self.portfolio_sequence)

        default_values = np.zeros((assets_count, 2, portfolio_count))
        prices_and_counts = dict(zip(self.assets_names, default_values))

        for portfolio_index, portfolio in enumerate(self.portfolio_sequence):
            date = portfolio.date.strftime(AssetsPortfolio.TIME_FORMAT)
            # This is pluuuuug
            date = "2017-01-09"
            for asset in self.assets_names:
                row = self.get_prices_by_date(asset, date)
                prices_and_counts[asset][0][portfolio_index] = \
                    row['Open'].iloc[0]
                prices_and_counts[asset][1][portfolio_index] = \
                    portfolio.count[self.get_asset_index(asset)]

        x_val = map(lambda p: p.date.strftime(AssetsPortfolio.TIME_FORMAT),
                    self.portfolio_sequence)[1:]

        graph_data = prices_and_counts
        for asset in self.assets_names:
            prices_and_counts_of_one_asset = prices_and_counts[asset]
            value = [(functor.apply_graph(prices_and_counts_of_one_asset,
                                          self.init_cash),
                      functor.apply_value(prices_and_counts_of_one_asset,
                                          self.init_cash))
                     for functor in functors]
            graph_data[asset] = value

        functor_names = [functor.get_name() for functor in functors]

        generate_report(x_values=x_val, path=path,
                        dict_of_lists_of_y_values=graph_data,
                        balance_history=self.balance_history,
                        graph_names=functor_names)

    def evaluate_portfolio(self):
        current_state = self.portfolio_sequence[-1].count
        return np.sum(current_state * self.prices)
