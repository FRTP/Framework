from configparser import ConfigParser
from datetime import datetime
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
        self.train_period_start, self.train_period_end = \
            parser.get('data', 'train_period').split('-')
        self.test_period_start, self.test_period_end = \
            parser.get('data', 'test_period').split('-')
        assets = parser.get('data', 'assets').split(',')
        self.assets = [x.strip().strip("'") for x in assets]
        self.start_money = parser.get('money', 'start_money')
        self.srv_ip = parser.get('server', 'ip')
        self.srv_login = parser.get('server', 'login')
        self.srv_pwd = parser.get('server', 'password')
        self.srv_port = parser.get('server', 'port')

    def get_algo_name(self):
        return self.algo_name

    def get_algo_path(self):
        return self.algo_path

    def get_train_period(self):
        return datetime.strptime(self.train_period_start, '%d.%m.%Y'),\
               datetime.strptime(self.train_period_end, '%d.%m.%Y')

    def get_test_period(self):
        return datetime.strptime(self.test_period_start, '%d.%m.%Y'), \
               datetime.strptime(self.test_period_end, '%d.%m.%Y')

    def get_assets_list(self):
        return self.assets

    def get_start_money(self):
        return int(self.start_money)

    def get_srv_ip(self):
        return self.srv_ip

    def get_srv_port(self):
        return int(self.srv_port)

    def get_srv_login(self):
        return self.srv_login

    def get_srv_pwd(self):
        return self.srv_pwd
