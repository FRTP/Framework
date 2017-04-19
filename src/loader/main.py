from loader import Loader
from sys import argv


def main():
    if len(argv) > 1:
        Loader(argv[1])
    else:
        Loader()


main()
