#include "cdaemon.h"
#include "cparser.h"
#include "exception.hpp"

int main(int argc, char** argv) {
	try {
		CParser parser(argc, argv);
		parser.parse();

		CDaemon daemon(parser); 
		int ret = daemon.start();
	}
	catch(IException* e) {
		e->what();
	}

	return 0;
}
