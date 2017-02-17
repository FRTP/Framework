import ctypes
lib = ctypes.cdll.LoadLibrary('client/libfrtpsrv.so')

class CClient(object):
    def __init__(self, server, port):
        self.obj = lib.create_instance(server, port)
    def get_file_list(self):
        return lib.get_file_list(self.obj)
    def get_file(self, filename, newfilename):
        return lib.get_file(self.obj, ctypes.c_char_p(filename), ctypes.c_char_p(newfilename))
    def clear(self):
        lib.clear(self.obj)
