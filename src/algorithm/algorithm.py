import numpy as np


class Algorithm:
    def __init__(self):
        pass

    def fit(self, pd_frames_list, names_list):
        # Here should be some clever training
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

    def load(self, path):
        raise NotImplementedError

    def save(self, path):
        raise NotImplementedError


# save model params to file

# np.savez('model.npz', *lasagne.layers.get_all_param_values(network))
# # load pretrained model params from file
#
# with np.load('model.npz') as f:
#     param_values = [f['arr_%d' % i] for i in range(len(f.files))]
# lasagne.layers.set_all_param_values(network, param_values)


class LasagneAlgo(Algorithm):

    def compile_network(self):
        raise NotImplementedError


class TensorflowAlgo(Algorithm):
    pass
