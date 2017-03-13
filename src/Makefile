all: daemon

daemon: common server client

common:
	@$(MAKE) -C daemon

server: common
	@$(MAKE) -C daemon/server

client: common
	@$(MAKE) -C daemon/client

clean:
	rm daemon/build/*.o daemon/client/build/*.o daemon/server/build/*.o
