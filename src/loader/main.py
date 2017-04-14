from loader import Loader
from sys import argv

if len(argv > 1):
    l = Loader(argv[1])
else:
    l = Loader()
