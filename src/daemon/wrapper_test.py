import datetime
import wrapper
import unittest


class TestServerAPI(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.client = wrapper.CClient('127.0.0.1', 34567,
                                     'user',  # 'password here',
                                     '/home/maked0n/frtp/',
                                     'data/')

    def test_getbyname(self):
        self.client.get_file('BCS\tAFLT/test_data.csv',
                             'BCS\tAFLT/test_data.csv',
                             'ASSETS', True)
        all_ok = self.client.check_integrity('BCS\tAFLT/test_data.csv',
                                             'BCS\tAFLT/test_data.csv',
                                             'ASSETS')
        self.assertTrue(all_ok)

    def test_getnonexistent(self):
        with self.assertRaises(RuntimeError):
            self.client.get_file('BCS\tAFLT/nonexistent.file',
                                 'BCS\tAFLT/nonexistent.file',
                                 'ASSETS', True)

    def test_sendbyname(self):
        self.client.upload_file('BCS\tAFLT/test_send', 'ASSETS')
        all_ok = self.client.check_integrity('BCS\tAFLT/test_send',
                                             'BCS\tAFLT/test_send',
                                             'ASSETS')
        self.assertTrue(all_ok)

    def test_sendnonexistent(self):
        with self.assertRaises(RuntimeError):
            self.client.upload_file('AFLT/nonexistent.file',
                                    'ASSETS')

    def test_getperiod(self):
        converter = wrapper.CConvertFromDate(datetime.date(2015, 8, 7),
                                             datetime.date(2015, 8, 10),
                                             'ASSETS', 'BCS', 'AFLT')
        try:
            self.client.get_info(converter, force=True)
        except wrapper.ExInvalidMD5:
            self.fail("Invalid MD5")

    def test_getfromyahoo(self):
        converter = wrapper.CConvertFromDate(datetime.date(2010, 8, 7),
                                             datetime.date(2015, 8, 10),
                                             'ASSETS', 'YAHOO', 'YHOO')
        self.client.get_info(converter, force=True)

    def test_invalidlogin(self):
        with self.assertRaises(Exception):
            self.client = wrapper.CClient('127.0.0.1', 34567,
                                          'no_such_user', 'password',
                                          '/home/maked0n/frtp/')


if __name__ == '__main__':
    unittest.main()
