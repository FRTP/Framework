import os.path
import datetime
import libfrtpsrv as lib
from abc import ABCMeta, abstractmethod


class ExInvalidMD5(Exception):
    def __init__(self):
        Exception.__init__(self)

DATA_TYPES = {'SHARES': 0, 'TWITTER': 1}


class IConverter(object):
    __metaclass__ = ABCMeta

    @abstractmethod
    def get_filenames(self):
        pass

    @abstractmethod
    def get_datatype(self):
        pass


class CConvertFromDate(IConverter):
    def __init__(self, date_from, date_to, data_type, subdirs):
        self.filelist = []
        self.data_type = data_type
        if data_type == 'SHARES':
            counter = date_from
            delta = datetime.timedelta(days=1)
            while counter <= date_to:
                self.filelist.append(subdirs + "/" +
                                     counter.strftime("%Y%m%d") + ".csv")
                counter += delta

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
    def __init__(self, srv_address, srv_port, working_dir):
        if working_dir[-1] != "/":
            working_dir += str("/")
        self.client = lib.LibClient(workingdir=working_dir)
        self.context = self.client.create_context()
        lib.LibClient.connect(server=srv_address, port=srv_port,
                              context=self.context)
        if not os.path.exists(working_dir):
            os.makedirs(working_dir)

    def get_file(self, filename, newfilename, data_type, force=False):
        l_arg = [filename, newfilename, str(force)]
        cmd = lib.LibCommandFactory.create_command("GetFile", l_arg)
        lib.LibClient.invoke(context=self.context, command=cmd,
                             datatype=DATA_TYPES[data_type])

    def upload_file(self, filename, data_type):
        cmd = lib.LibCommandFactory.create_command("UploadFile",
                                                   [filename])
        lib.LibClient.invoke(context=self.context, command=cmd,
                             datatype=DATA_TYPES[data_type])

    def get_md5(self, srv_filename, data_type):
        cmd = lib.LibCommandFactory.create_command("GetMD5",
                                                   [srv_filename])
        lib.LibClient.invoke(context=self.context, command=cmd,
                             datatype=DATA_TYPES[data_type])
        return CClient.get_hash(cmd)

    def check_integrity(self, srv_filename, cli_filename, data_type):
        return lib.LibClient.check_integrity(context=self.context,
                                             server=srv_filename,
                                             client=cli_filename,
                                             datatype=DATA_TYPES[data_type])

    def get_info(self, converter, check=True, force=False):
        for filename in converter.get_filenames():
            ret = self.get_file(filename, filename, converter.get_datatype(),
                                force)
            if ret == 0 and check:
                if not self.check_integrity(filename):
                    raise ExInvalidMD5()
