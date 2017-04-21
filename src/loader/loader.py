from config_parser import Config
from pandas import read_csv
from Environment import Environment
from datetime import datetime
from shutil import copyfile
from os import chdir, mkdir
from os.path import exists
from wrapper import CClient, CConvertFromDate


def _download_data(period, working_dir, assets_list, srv_ip, srv_port,
                   srv_login, srv_password):
    if not exists(working_dir):
        mkdir(working_dir)
    chdir(working_dir)
    client = CClient(srv_ip, srv_port, srv_login, srv_password, '.', '.')
    for asset in assets_list:
        converter = CConvertFromDate(period[0], period[1], 'ASSETS', 'YAHOO',
                                     asset)
        client.get_info(converter)
    chdir('..')


def _create_dataframes(working_dir, assets_list):
    data = []
    chdir(working_dir)
    for asset in assets_list:
        data.append(read_csv('assets/YAHOO/' + asset + '/data.csv'))
    chdir('..')
    return data


def _import_algorithm(algo_path, algo_name):
    copyfile(algo_path, './' + algo_name)
    module = __import__(algo_name)
    return getattr(module, algo_name)()


class Loader:

    def __init__(self, config_filename='config.cfg'):
        config = Config(config_filename)

        # importing algorithm module
        # class name == file name
        alg = _import_algorithm(config.get_algo_path(), config.get_algo_name())

        # downloading CSVs from server
        srv_ip = config.get_srv_ip()
        srv_port = config.get_srv_port()
        srv_pwd = config.get_srv_pwd()
        srv_login = config.get_srv_login()
        assets_list = config.get_assets_list()
        _download_data(config.get_test_period(), 'test', assets_list,
                       srv_ip, srv_port, srv_login, srv_pwd)
        _download_data(config.get_train_period(), 'train', assets_list,
                       srv_ip, srv_port, srv_login, srv_pwd)
        print('downloaded data')
        train_dataframes = _create_dataframes('train', assets_list)
        test_dataframes = _create_dataframes('test', assets_list)
        print('created dataframes')

        # initializing Environment
        env = Environment(train_dataframes, assets_list,
                          initial_balance=config.get_start_money())
        print('created Environment')

        # training
        alg.fit(train_dataframes, assets_list)
        print('finished fitting')

        # trading
        print(test_dataframes[0].head(5))
        trading_len = len(test_dataframes[0])
        for i in range(trading_len - 1, -1, -1):
            cur_date = datetime.strptime(test_dataframes[0].iloc[i]['Date'],
                                         '%Y-%m-%d')
            print('cur_date = ' + str(cur_date))
            to_buy_list = alg.step([x.iloc[i] for x in test_dataframes],
                                   assets_list, env.get_current_balance())
            env.buy(assets_list, to_buy_list, cur_date)
        env.generate_report([], None)


Loader()
