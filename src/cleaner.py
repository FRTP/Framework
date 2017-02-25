import os
import calendar

working_dir = "/var/frtp/data/"
dir_list = os.listdir(working_dir)
for directory in dir_list:
    cur_list = os.listdir(working_dir + directory)
    for fl in cur_list:
        year = int(fl[:4])
        month = int(fl[4:6])
        day = int(fl[6:8])
        weekday = calendar.weekday(year, month,  day)
        if weekday == 5 or weekday == 6:
            os.remove(working_dir + directory + "/" + fl)
