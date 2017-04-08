import os.path
import datetime
import libfrtpsrv as lib
from abc import ABCMeta, abstractmethod


class ExInvalidMD5(Exception):
    def __init__(self):
        Exception.__init__(self)


DATA_TYPES = {'ASSETS': 0, 'TWITTER': 1}


class IConverter(object):
    __metaclass__ = ABCMeta

    @abstractmethod
    def get_filenames(self):
        pass

    @abstractmethod
    def get_datatype(self):
        pass


class CConvertFromDate(IConverter):
    def __init__(self, date_from, date_to, data_type, source, subdirs):
        self.filelist = []
        self.data_type = data_type
        if data_type == 'ASSETS' and source == 'BCS':
            counter = date_from
            delta = datetime.timedelta(days=1)
            while counter <= date_to:
                self.filelist.append(["BCS\t" + subdirs + "/" +
                                      counter.strftime("%Y%m%d") + ".csv",
                                      "BCS\t" + subdirs + "/" +
                                      counter.strftime("%Y%m%d") + ".csv"])
                counter += delta
        elif data_type == 'ASSETS' and source == 'YAHOO':
            s_date_from = 'a=' + str(date_from.month - 1) + '&b=' +\
                          str(date_from.day) + '&c=' +\
                          str(date_from.year) + '&'
            s_date_to = 'd=' + str(date_to.month - 1) + '&e=' +\
                        str(date_to.day) + '&f=' +\
                        str(date_to.year)
            self.filelist.append(["YAHOO\t" + subdirs + "/" +
                                  s_date_from + s_date_to,
                                  "YAHOO\t" + subdirs + "/data.csv"])

    def get_filenames(self):
        return self.filelist

    def get_datatype(self):
        return self.data_type


class CNoConversion(IConverter):
    def __init__(self, filename):
        self.filelist = [filename]

    def get_filenames(self):
        return self.filelist

    def get_datatype(self):
        return self.data_type


class CClient(object):
    def __init__(self, srv_address, srv_port, srv_login, srv_password,
                 working_dir, data_subdir):
        if working_dir[-1] != "/":
            working_dir += str("/")
        if data_subdir[-1] != "/":
            data_subdir += str("/")
        self.client = lib.LibClient(workingdir=working_dir,
                                    datasubdir=data_subdir)
        self.client.connect(server=srv_address, port=srv_port,
                            login=srv_login, password=srv_password)
        if not os.path.exists(working_dir):
            os.makedirs(working_dir)

    def get_file(self, filename, newfilename, data_type, force=False):
        l_arg = [filename, newfilename, str(force)]
        cmd = lib.LibCommandFactory.create_command("GetFile", l_arg)
        self.client.invoke(command=cmd, datatype=DATA_TYPES[data_type])

    def upload_file(self, filename, data_type):
        cmd = lib.LibCommandFactory.create_command("UploadFile",
                                                   [filename])
        self.client.invoke(command=cmd, datatype=DATA_TYPES[data_type])

    def get_md5(self, srv_filename, data_type):
        cmd = lib.LibCommandFactory.create_command("GetMD5",
                                                   [srv_filename])
        self.client.invoke(command=cmd, datatype=DATA_TYPES[data_type])
        return CClient.get_hash(cmd)

    def check_integrity(self, srv_filename, cli_filename, data_type):
        return self.client.check_integrity(server=srv_filename,
                                           client=cli_filename,
                                           datatype=DATA_TYPES[data_type])

    def get_info(self, converter, check=True, force=False):
        for filenames in converter.get_filenames():
            try:
                self.get_file(filenames[0], filenames[1],
                              converter.get_datatype(),
                              force)
            except RuntimeError:
                pass
            if check:
                if os.path.isfile(filenames[1]):
                    if not self.check_integrity(filenames[0], filenames[1],
                                                converter.get_datatype()):
                        raise ExInvalidMD5()
