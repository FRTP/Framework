import wrapper

client = wrapper.CClient("192.168.88.242", 34567, "/home/maked0n/frtp/", "frtpclient.so")
print(client.get_file_list())
del client
