from loader import Loader
from sys import argv


def main():
    if len(argv) > 1:
        Loader(argv[1])
    else:
        Loader()
# main()

import pandas as pd
df = pd.read_csv("linear_train.txt", names=['Word', 'Really?'])
print(df.head())

rows = df.loc[df['Word'] == 'Аарон']

import numpy as np
print(np.array(rows['Really?']))

price_count = np.zeros((2, 4, 6))
print(price_count)

print(df.head(1).to_dict())

