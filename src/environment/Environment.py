from datetime import datetime
import numpy as np


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
        index = all_assets_names.tolist().index(asset_name)
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
    #  __                                                                      __
    # |                                                                          |
    # |   __________________________________________________________________     |
    # |  | Time                | Open | High | Low | Close | Volume | Count |    |
    # |  | --------------------|------|------|-----|-------|--------|-------|    |
    # |  | 05.11.1996 13:15:30 | 32.2 | 45.7 | ... |  ...  |  ....  |  ...  |    |
    # |  |---------------------|------|------|-----|------------------------|    |
    # |  | %d.%m.%Y %H:%M:%S   | ...  | ...  | ... |  ...  |  ....  |  ...  |    |
    # |                                                                          |
    # |                                                                          |
    #  --                                                                      --
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
                 initial_balance=0):

        if initial_assets_count is None:
            initial_assets_count = np.zeros(len(hist_data))
        if current_assets_prices is None:
            current_assets_prices = np.zeros(len(hist_data))
        if initial_date is None:
            initial_date = datetime.now()

        self.names = names_list
        self.prices = current_assets_prices
        self.current_balance = initial_balance
        self.balancing_coefficients = \
            self.compute_balancing_coefficients(hist_data)
        assets_count = np.array(initial_assets_count)
        self.portfolio_sequence = [AssetsPortfolio(assets_count, initial_date)]

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

        return np.ones(len(self.names))

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
            current_asset_idx = self.names.tolist().index(given_names[i])
            current_coef = self.balancing_coefficients[current_asset_idx]
            new_assets_count[current_asset_idx] += \
                given_count[i] * current_coef
            self.current_balance -= \
                self.prices[current_asset_idx] * given_count[i] * current_coef

        new_portfolio = AssetsPortfolio(new_assets_count, purchase_date)
        self.portfolio_sequence.append(new_portfolio)

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
        needed_index = self.names.tolist().index(asset_name)
        return self.portfolio_sequence[-1].count[needed_index]

    # @brief
    #   Updates price of the one asset
    def update_price(self, asset, new_price):
        index = self.names.tolist().index(asset)
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
    def update_prices(self, assets_names, new_prices):
        if len(assets_names) != len(new_prices):
            raise Exception("assets_names & new_prices "
                            "must have the same size")
        for i in range(len(assets_names)):
            self.update_price(assets_names[i], new_prices[i])

    # @brief
    #   Extracts asset's price at needed_date from asset_hist_data
    #
    # @param asset_hist_data <pd dataframe>
    #   Dataframe with historical prices for needed asset.
    #       Needed format noticed in constructor.
    #
    # @param needed_date <str> in following format :%Y-%m-%dT%H:%M:%S
    #    or <datetime>.
    #
    # @throws ValueError
    #   If needed_date has wrong format (not AssetsPortfolio.TIME_FORMAT)
    #
    # @throws TypeError
    #   If needed_date is not <str> or <datetime>
    #
    # @returns
    #   Needed asset price - if found.
    #   None - if wasn't found.
    #
    @staticmethod
    def get_price_by_date(asset_hist_data, needed_date):
        needed_date = AssetsPortfolio.to_datetime(needed_date)
        idx = Environment.__find__row__by__date__(asset_hist_data, needed_date)

        if idx is not None:
            return idx[1]
        else:
            return None

    # Finds row in pd_frame of a particular asset by date
    @staticmethod
    def __find__row__by__date__(asset_hist_data, given_date):
        if type(given_date) != datetime:
            raise TypeError("given_date must be <datetime> type.")

        for i in range(len(asset_hist_data)):
            cur_date = AssetsPortfolio.to_datetime(asset_hist_data[:, 0][i])
            if cur_date == given_date:
                return i
        return None

    # Gives rows in the asset_hist_data
    #   which are appropriate to the start_date and length of the period.
    @staticmethod
    def get_train_period(asset_hist_data, start_date=None, length=0, ratio=0.):
        if length == 0 and ratio == 0:
            return None

        if start_date is None:
            first_idx = 0
        else:
            start_date = AssetsPortfolio.to_datetime(start_date)
            first_idx = Environment.__find__row__by__date__(asset_hist_data,
                                                            start_date)
            if first_idx is None:
                raise ValueError("No such start_date in asset_hist_data.")

        if length == 0:
            length = len(asset_hist_data) * ratio
        last_idx = min(len(asset_hist_data), first_idx + length)

        return asset_hist_data[first_idx:last_idx]

    # Just gives names of considered assets.
    def get_assets_names(self):
        return self.names

    # Environments maintains current balance
    #   and can be asked for it at any moment.
    def get_current_balance(self):
        return self.current_balance

    # Gives the last portfolio in the portfolio chain (sequence).
    def get_current_portfolio(self):
        return self.portfolio_sequence[-1]
