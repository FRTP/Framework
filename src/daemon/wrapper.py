import ctypes
import os.path
import csv


class CClient(object):
    def __init__(self, server, port, working_dir, lib_path):
        self.working_dir = working_dir
        self.lib = ctypes.cdll.LoadLibrary(lib_path)
        srv_str = ctypes.c_char_p(str(server).encode('utf-8'))
        self.obj = self.lib.create_instance(srv_str, port)
        if not os.path.exists(working_dir):
            os.makedirs(working_dir)

    def get_file_list(self):
        return self.lib.get_file_list(self.obj)

    def get_file(self, filename, newfilename, delimit=';', quotech='|'):
        if not os.path.exists(newfilename):
            filename_s = ctypes.c_char_p(str(filename).encode('utf-8'))
            newfilename_s = ctypes.c_char(str(newfilename).encode('utf-8'))
            self.lib.get_file(self.obj, filename_s, newfilename_s)
        csvfile = open(newfilename, 'rb')
        return csv.reader(csvfile, delimiter=delimit, quotechar=quotech)

    def get_info(self, name, date, delimit=';', quotech='|'):
        if not os.path.exists(self.working_dir + name):
            os.makedirs(self.working_dir + name)
        filename = name + "/" + date + ".csv"
        return self.get_file(filename, self.working_dir + filename, delimit,
                             quotech)
