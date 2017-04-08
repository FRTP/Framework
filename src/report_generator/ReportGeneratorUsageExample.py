import sys
import numpy as np
from ReportGenerator import generate_report

#  some custom functors


class SumFunctor:
    def __init__(self):
        self.name = "sum_functor"

    @staticmethod
    def apply(np_array):
        return np.sum(np_array)

    def get_name(self):
        return self.name


class MeanFunctor:
    def __init__(self):
        self.name = "mean_functor"

    @staticmethod
    def apply(np_array):
        return np_array.mean()

    def get_name(self):
        return self.name


report_path = ""
if len(sys.argv) > 1:
    report_path = sys.argv[1]
else:
    report_path = "report.pdf"

input_array = np.linspace(0, 1, 230)
our_functors = [SumFunctor(), MeanFunctor()]

generate_report(input_array, our_functors, report_path)
