from configparser import ConfigParser
import os
import errno


class Config:

    def __init__(self, config_file_name):
        parser = ConfigParser()
        if not parser.read(config_file_name):
            raise FileNotFoundError(errno.ENOENT, os.strerror(errno.ENOENT),
                                    config_file_name)
        self.algo_name = parser.get('algorithm', 'algo_name')
        self.algo_path = parser.get('algorithm', 'algo_path')
        self.training_period_start, self.training_period_end = \
            parser.get('data', 'training_period').split('-')
        assets = parser.get('data', 'assets').split(',')
        self.assets = [x.strip().strip("'") for x in assets]
        self.start_money = parser.get('money', 'start_money')

    def get_algo_name(self):
        return self.algo_name

    def get_algo_path(self):
        return self.algo_path

    def get_training_period(self):
        return self.training_period_start, self.training_period_end

    def get_assets_list(self):
        return self.assets

    def get_start_money(self):
        return self.start_money
