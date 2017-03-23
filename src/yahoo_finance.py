import datetime
import pandas as pd
import urllib.request


class CChecker:
    def __init__(self, working_dir, shares_lst):
        self.working_dir = working_dir
        self.standart_filename = shares_lst[0] + ".csv"
        self.standart_dates = []
        fd = pd.read_csv(self.working_dir + self.standart_filename)
        self.standart_dates = fd.Date

    def check(self, filename):
        fd = pd.read_csv(filename)
        new_set = set(fd.Date)
        standart_set = set(self.standart_dates)
        return new_set ^ standart_set
        

start_date = 'a=00&b=01&c=2000'
now = datetime.datetime.now()

shares_list = [
        'INTC',
        'MDT',
        'MSFT',
        'AAPL',
        'ADBE',
        'AMD',
        'CSCO',
        'EBAY',
        'EA',
        'NTAP',
        'NVDA',
        'ORCL',
        'SYMC',
        'YHOO',
        'XRX'
        ]

def open_url(share):
    end_date = "d=" + str(now.month - 1) + "&e=" + str(now.day) + \
            "&f=" + str(now.year)
    response = urllib.request.urlopen(
            'http://ichart.finance.yahoo.com/table.csv?s=' + share +
            '&' + start_date + '&' + end_date + '&g=d&ignore=.csv')
    return response.read()

def check_identity():
    standart_filename = shares_list[0] + ".csv"
    print("Standart file: " + standart_filename)

def get_info(working_dir):
    if working_dir[-1] != "/":
        working_dir += str("/")

    outfile = open(working_dir + shares_list[0] + ".csv", 'wb+')
    outfile.write(open_url(shares_list[0]))
    checker = CChecker(working_dir, shares_list)

    for i in range(1, len(shares_list)):
        filename = working_dir + shares_list[i] + ".csv"
        outfile = open(filename, 'wb+')
        outfile.write(open_url(shares_list[i]))
        test_set = checker.check(filename)
        if len(test_set) != 0:
            print("Difference in " + shares_list[i] + ".csv:\n" + str(test_set))
            return


get_info('/home/maked0n/frtp/yahoo')
