from ConfigParser import SafeConfigParser


class ConfigParser:

    def _read_opt(self, section, option):
        if self.parser.has_option(section, option):
            return self.parser.get(section, option)
        else:
            print('No option ' + option + 'in section ' + section)
            exit()

    def __init__(self, config_file_name):
        self.parser = SafeConfigParser()
        if not self.parser.read(config_file_name):
            print("Wrong filename")
            exit()
        self.algo_name = self._read_opt('algorithm', 'algo_name')
        self.algo_path = self._read_opt('algorithm', 'algo_path')
        self.training_period_start, self.training_period_end = \
            self._read_opt('data', 'training_period').split('-')
        assets = self._read_opt('data', 'assets').split(',')
        self.assets = [x.strip().strip("'") for x in assets]
        self.start_money = self._read_opt('money', 'start_money')

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
