import os.path
import datetime
import hashlib
from libfrtpsrv import *


class ExNoFile(Exception):
    def __init__(self):
        Exception.__init__(self)


class ExInvalidMD5(Exception):
    def __init__(self):
        Exception.__init__(self)


class CClient(object):
    def __init__(self, srv_address, srv_port, working_dir, lib_path):
        self.client = LibClient(server=srv_address, port=srv_port)
        self.context = self.client.create_context()
        LibClient.connect(context=self.context)
        self.working_dir = working_dir
        if not os.path.exists(working_dir):
            os.makedirs(working_dir)

    def get_file(self, filename, newfilename, force=False):
        if not os.path.exists(newfilename) and not force:
            cmd = LibCommandFactory.create_command(cmd_id="GetFile",
                                                   args=[filename, newfilename])
            LibClient.invoke(context=self.context, command=cmd)

    def get_md5(self, filepath):
        cmd = LibCommandFactory.create_command(cmd_id="GetMD5",
                                               args=[filepath])
        LibClient.invoke(context=self.context, command=cmd)
        return CClient.get_hash(cmd)

    def check_integrity(self, filepath):
        if not os.path.exists(filepath):
            raise ExNoFile()
        cur_md5 = hashlib.md5(open(filepath, 'rb').read()).hexdigest()
        server_md5 = get_md5(filepath)
        return cur_md5 == server_md5

    def get_info(self, name, date_from, date_to, check=True, force=False):
        if not os.path.exists(self.working_dir + name):
            os.makedirs(self.working_dir + name)
        counter = date_from
        delta = datetime.timedelta(days=1)
        while counter <= date_to:
            filename = name + "/" + counter.strftime("%Y%m%d") + ".csv"
            ret = self.get_file(filename, self.working_dir + filename, force)
            if ret == 0 and check:
                if not self.check_integrity(self.working_dir + filename):
                    raise ExInvalidMD5()
            counter += delta
