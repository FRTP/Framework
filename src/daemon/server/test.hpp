#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <fstream>
#include <stdio.h>
#include <string>

#include "../utility.h"

BOOST_AUTO_TEST_SUITE(TestMD5Sum)

BOOST_AUTO_TEST_CASE(Equal) {
	std::string full_path = "test.dat"; 
	std::ofstream out(full_path);
	out << "test string";
	out.close();

	utility::md5sum_ptr md5 = utility::calculate_md5(full_path);

	std::string command = "md5sum " + full_path + " | grep -Eo '^.{32}'";
	std::string etalon_md5 = "";
	const int MAX_BUFF_SIZE = 1024;
	char etalon_md5_buf[MAX_BUFF_SIZE];
	FILE* stream = popen(command.c_str(), "r");
	if (stream) {
		fgets(etalon_md5_buf, MAX_BUFF_SIZE, stream);
		etalon_md5 = std::string(etalon_md5_buf);
		pclose(stream);
	}

	BOOST_CHECK_EQUAL(utility::md5sum_to_str(md5), etalon_md5);
}

BOOST_AUTO_TEST_SUITE_END()
