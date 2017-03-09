#!/usr/bin/python

import numpy as np
import pandas as pd
from Environment import Environment


# Extract historical data.
historical_data = pd.read_csv('historical_data_example.csv')


# Generate some initials for environment.
stocks_names = historical_data.columns
initial_stocks_count = np.arange(len(stocks_names))
current_stocks_prices = np.linspace(30, 90, len(stocks_names))
initial_balance = 100500

history_size = 3


print("Initial balance :", initial_balance)
print("Stocks names :", stocks_names)
print("Stocks prices :", current_stocks_prices)
print("Stocks count :", initial_stocks_count)
print(historical_data)
print()

# Create environment with some initials
our_environment = Environment(historical_data, initial_stocks_count,
                              current_stocks_prices, initial_balance)


# Print all stocks we currently have according to the environment
def print_stocks_we_have(env):
    names = env.get_stocks_names()
    for name in names:
        print("{} count: {}".format(name, env.get_stock_count(name)))
    print("Current balance : {}".format(env.get_current_balance()))
    print("-----------------------")
    return


# Print all stocks
print_stocks_we_have(our_environment)

# Buy some stocks.
our_environment.buy(["Sberbank", "Gazprom"], np.array([18, 10]))

# Print all stocks
print_stocks_we_have(our_environment)

# Sell some stocks.
our_environment.sell(["Gazprom", "Rostelecom"], np.array([7, 6]))

# Print all stocks
print_stocks_we_have(our_environment)
