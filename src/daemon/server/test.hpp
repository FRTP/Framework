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
	std::string standart_md5 = "";
	const int MAX_BUFF_SIZE = 1024;
	char standart_md5_buf[MAX_BUFF_SIZE];
	FILE* stream = popen(command.c_str(), "r");
	if (stream) {
		fgets(standart_md5_buf, MAX_BUFF_SIZE, stream);
		standart_md5 = std::string(standart_md5_buf);
		standart_md5.pop_back(); //remove \n
		pclose(stream);
	}

	BOOST_CHECK_EQUAL(utility::hash_to_str(md5), standart_md5);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(TestSHA512)

BOOST_AUTO_TEST_CASE(Equal) {
	std::string test_str = "test string";
	std::string res = std::string("10e6d647af44624442f388c2c14a787ff8b17e6165b83d767ec047768d8cbcb7") +
			  std::string("1a1a3226e7cc7816bc79c0427d94a9da688c41a3992c7bf5e4d7cc3e0be5dbac");
	utility::sha512_ptr sha_hash = utility::encrypt_string(test_str);
	std::string sha_str = utility::hash_to_str(sha_hash);

	BOOST_CHECK_EQUAL(res, sha_str);
}

BOOST_AUTO_TEST_SUITE_END()
