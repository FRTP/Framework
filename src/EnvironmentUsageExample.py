#!/usr/bin/python

import numpy as np
from Environment import Environment, StockPortfolio


# Generate some initials for environment
stocks_names = ["sber", "rostelecom", "gazprom"]
initial_stocks_count = np.arange(len(stocks_names))
current_stocks_prices = np.linspace(30, 90, 3)
initial_balance = 100500

history_size = 3
historical_data = [(name, np.arange(history_size)*len(name))
                   for name in stocks_names]


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
our_environment.buy(["sber", "gazprom"], np.array([18, 10]))

# Print all stocks
print_stocks_we_have(our_environment)

# Sell some stocks.
our_environment.sell(["gazprom", "rostelecom"], np.array([7, 6]))

# Print all stocks
print_stocks_we_have(our_environment)
