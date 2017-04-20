import pytz
import pandas as pd
from datetime import datetime
from zipline.algorithm import TradingAlgorithm
import sys
import getopt
import re
from user_defined import initialize, handle_data, save, load, analyze


def check_important_columns(data):
    for df in data:
        if 'volume' not in df.axes[1].values:
            default = 10000
            df['volume'] = default
            print "Can't find column 'volume' needed for calculations, " \
                  "column 'volume' with default values(%i) added" % default

        if 'price' not in df.axes[1].values:
            df['price'] = df['close']
            print "Can't find column 'price', " \
                  "column 'close' will be used as price"


def make_panels(universe):
    if universe.parser:
        def parser_func(x):
            return datetime.strptime(x, universe.parser)
        data = [pd.read_csv(filename, index_col=0, date_parser=parser_func)
                for filename in universe.data_files]
    else:
        data = [pd.read_csv(filename, index_col=0, parse_dates=True)
                for filename in universe.data_files]

    for dt in data:
        dt.index = dt.index.tz_localize(pytz.UTC)

    check_important_columns(data)

    if len(universe.symbols) == 0:
        universe.symbols = ["STOCK_" + str(i) for i in range(len(data))]

    dict_data = dict()
    for stock, dt in zip(universe.symbols, data):
        dict_data[stock] = dt

    pn = pd.Panel(dict_data)
    if universe.training_size < 1:
        training_size = int(pn.shape[1] * universe.training_size)
    else:
        training_size = int(universe.training_size)
    training_pn = pn[:, :training_size, :]
    test_pn = pn[:, training_size:, :]
    return training_pn, test_pn


def wrapped_init(context):
    context.panel = training_panel
    if universe.load_file != "":
        load(universe, context)
    else:
        initialize(context)
    if universe.save_file != "":
        save(universe, context)


class Universe:
    def __init__(self):
        self.data_files = ""
        self.save_file = ""
        self.load_file = ""
        self.frequency = ""
        self.capital_base = 200000
        self.parser = None
        self.symbols = []
        self.training_size = None

    def print_params(self):
        print "Universe parameters:"
        print "Data loaded from: ", self.data_files
        if self.save_file != "":
            print "Trained params will be saved to:", self.save_file
        else:
            print "Trained params will not be saved"
        if self.load_file != "":
            print "Trained params will be loaded from:", self.load_file
        else:
            print "Trained params will not be loaded", self.load_file
        print "Frequency:", self.frequency
        print "Starting capital:", self.capital_base
        print "Parser string:", self.parser if self.parser \
            else "automatically chosen"
        print "Symbols:", self.symbols if len(self.symbols) > 0 \
            else "automatically generated"
        print "Training size:", self.training_size
        print

    def check_params(self):
        correct = True

        if self.data_files == "":
            correct = False
            print "You must specify path to each used csv " \
                  "by passing command line argument" \
                  " <--data \"path/to/file1.csv path/to/file2.csv\">"

        if not (self.frequency in ["minute", "daily"]):
            correct = False
            print "Frequency must be either 'minute' or 'daily'"

        if self.save_file != "" and self.load_file != "":
            correct = False
            print "You can't use trained parameters from file " \
                  "and save them to file at the same time"

        if self.training_size is None:
            correct = False
            print "You must specify training sample size " \
                  "via training_size parameter"
        if 0 > self.training_size:
            correct = False
            print "Training size must be either float in [0,1) or int >= 2"

        return correct

    def extract_parameters(self, print_values=True):
        argv = sys.argv[1:]
        try:
            args_to_read = ["data=", "frequency=", "capital_base=",
                            "symbols=", "save_trained_params=",
                            "load_trained_params=", "parser=",
                            "training_size="]
            opts, args = getopt.getopt(argv, "", args_to_read)
            if len(args) > 0:
                print "Can't recognize argument \"%s\"" % args[0]
                sys.exit(2)
        except getopt.GetoptError:
            print 'Error while parsing command-line arguments'
            sys.exit(2)
        for opt, arg in opts:
            if opt == "--data":
                self.data_files = re.split(" |,", arg)
            if opt == "--frequency":
                self.frequency = arg
            if opt == "--capital_base":
                self.capital_base = float(arg)
            if opt == "--symbols":
                self.symbols = re.split("\W+", arg)
            if opt == "--save_trained_params":
                self.save_file = arg
            if opt == "--load_trained_params":
                self.load_file = arg
            if opt == "--parser":
                self.parser = arg
            if opt == "--training_size":
                self.training_size = float(arg)
        if not self.check_params():
            sys.exit(2)
        if print_values:
            universe.print_params()


if __name__ == "__main__":
    universe = Universe()
    universe.extract_parameters()

    training_panel, test_panel = make_panels(universe)

    algo = TradingAlgorithm(initialize=wrapped_init,
                            handle_data=handle_data,
                            data_frequency=universe.frequency,
                            capital_base=universe.capital_base,
                            instant_fill=True)

    print "Timestamps in training:%i" % training_panel.shape[1]
    print "Timestamps in test:%i" % test_panel.shape[1]
    performance = algo.run(test_panel)

    analyze(results=performance, universe=universe)
