import numpy as np


class Algorithm:
    def __init__(self):
        pass

    def fit(self, pd_frames_list, names_list):
        # Here must be some clever training
        #   algorithm according to historical data.

        # todo : something clever.
        pass

    def step(self, pd_frame_row_list, names_list, current_balance):

        # use_somehow_pd_frame_row_list = pd_frame_row_list
        # use_somehow_current_balance = current_balance

        # We'll use a dumb way to buy assets for example at each step.
        assets_to_buy = names_list
        counts_to_buy = np.ones(len(names_list))

        return [assets_to_buy, counts_to_buy]
