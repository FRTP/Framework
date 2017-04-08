import numpy as np
import pandas as pd

from src.environment.Environment import Environment

path_to_data = '../temp/AAPL.csv'

# Extract historical data.
historical_data = pd.read_csv(path_to_data)


# Generate some initials for environment.
assets_names = ['Sberbank']
initial_assets_count = np.arange(len(assets_names))
current_assets_prices = np.linspace(30, 90, len(assets_names))
initial_balance = 100500


# print("Initial balance :", initial_balance)
# print("assets names :", assets_names)
# print("assets prices :", current_assets_prices)
# print("assets count :", initial_assets_count)
# print(historical_data)
print()


# Create environment with some initials
our_environment = Environment(hist_data=historical_data,
                              names_list=assets_names,
                              initial_assets_count=initial_assets_count,
                              current_assets_prices=current_assets_prices,
                              initial_date="2017-03-30",
                              initial_balance=initial_balance)


# Print all assets we currently have according to the environment
def print_assets_we_have(env):
    names = env.get_assets_names()
    for name in names:
        print("{} count: {}".format(name, env.get_asset_count(name)))
    print("Current balance : {}".format(env.get_current_balance()))
    print("-----------------------")
    return


# # Print all assets
# print_assets_we_have(our_environment)
#
# # Buy some assets.
# our_environment.buy(["Sberbank", "Gazprom"], np.array([18, 10]),
#                     "1994-11-05T13:15:30")
#
# # Print all assets
# print_assets_we_have(our_environment)
#
# # Sell some assets.
# our_environment.sell(["Gazprom", "Rostelecom"], np.array([7, 6]),
#                      "1994-11-05T14:15:30")
#
# # Print all assets
# print_assets_we_have(our_environment)
