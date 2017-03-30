import datetime
import wrapper
import unittest


class TestFileOperations(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.client = wrapper.CClient('127.0.0.1', 34567,
                                     'user', # 'password here',
                                     '/home/maked0n/frtp/')

    def test_getbyname(self):
        self.client.get_file('AFLT/test_data.csv', 'AFLT/test_data.csv',
                             'SHARES', True)
        all_ok = self.client.check_integrity('AFLT/test_data.csv',
                                             'AFLT/test_data.csv',
                                             'SHARES')
        self.assertTrue(all_ok)

    def test_getnonexistent(self):
        with self.assertRaises(RuntimeError):
            self.client.get_file('AFLT/nonexistent.file',
                                 'AFLT/nonexistent.file',
                                 'SHARES', True)

    def test_sendbyname(self):
        self.client.upload_file('test_send', 'SHARES')
        all_ok = self.client.check_integrity('test_send',
                                             'test_send',
                                             'SHARES')
        self.assertTrue(all_ok)

    def test_sendnonexistent(self):
        with self.assertRaises(RuntimeError):
            self.client.upload_file('AFLT/nonexistent.file',
                                    'SHARES')

    def test_getperiod(self):
        converter = wrapper.CConvertFromDate(datetime.date(2015, 8, 7),
                                             datetime.date(2015, 8, 10),
                                             'SHARES', 'AFLT')
        try:
            self.client.get_info(converter, force=True)
        except wrapper.ExInvalidMD5:
            self.fail("Invalid MD5")

    def test_invalidlogin(self):
        with self.assertRaises(Exception):
            cls.client = wrapper.CClient('127.0.0.1', 34567,
                                         'no_such_user', 'password',
                                         '/home/maked0n/frtp/')


if __name__ == '__main__':
    unittest.main()
