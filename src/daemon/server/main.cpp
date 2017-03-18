#ifndef TESTING
#include <stdexcept>
#include <iostream>

#include "cdaemon.h"
#include "cparser.hpp"

int main(int argc, char** argv) {
	try {
		CParser parser(argc, argv);
		parser.parse();

		CDaemon daemon(parser); 
		if (daemon.start() != 0) {
			std::cerr << "[EE]: Unable to start daemon" << std::endl;
		}
	}
	catch(std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}

#else
#include "test.hpp"

#endif //TESTING
