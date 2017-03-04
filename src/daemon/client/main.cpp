#include "cclient.h"

extern "C" {
	CClient* create_instance(char* server, int port) {
		boost::asio::io_service io_service;
		CClient* client = new CClient(io_service, server, port);
		io_service.run();
		return client;
	}

	int get_file(CClient* obj, char* filename, const char* newfile) {
		if (!obj) {
			return -1;
		}
		CCmdGetFile* cmd = new CCmdGetFile(std::string(filename), std::string(newfile));
		obj->invoke(cmd);
		delete cmd;
		return 0;
	}

	const unsigned char* get_md5(CClient* obj, char* filename) {
		if (!obj) {
			return NULL;
		}
		CCmdGetMD5* cmd = new CCmdGetMD5(std::string(filename));
		obj->invoke(cmd);
		const unsigned char* res = cmd->hash();
		delete cmd;
		return res;
	}

	void clear(CClient* obj) {
		if (obj) {
			delete obj;
		}
	}
}
