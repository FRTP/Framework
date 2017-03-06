import os
import csv


working_dir = "/var/frtp/data/"
dir_list = os.listdir(working_dir)
deleted_num = 0
for directory in dir_list:
    cur_list = os.listdir(working_dir + directory)
    for fl in cur_list:
        full_name = working_dir + directory + "/" + fl
        with open(full_name, 'r') as csv_file:
            next(csv_file)
            csv_reader = csv.reader(csv_file, delimiter=";")
            volume = sum(int(r[5]) for r in csv_reader)
            if volume == 0:
                os.remove(full_name)
                deleted_num += 1
print("Deleted " + str(deleted_num) + " files\n")
