#include "cclient.h"

extern "C" {
	CClient* create_instance(char* server, int port) {
		boost::asio::io_service io_service;
		CClient* client = new CClient(io_service, server, port);
		io_service.run();

		return client;
	}

	const char* get_file_list(CClient* obj) {
		if (!obj) {
			return NULL;
		}
		return obj->get_file_list();
	}

	int get_file(CClient* obj, char* filename, const char* newfile) {
		if (!obj) {
			return -4;
		}
		return obj->get_file(filename, newfile);
	}

	void clear(CClient* obj) {
		if (obj) {
			delete obj;
		}
	}
}
