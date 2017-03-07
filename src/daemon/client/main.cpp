#include <boost/python.hpp>

#include "cclient.h"

using namespace boost::python;

void translate_error(const ExError&);
void transtale_invalid_args_error(const ExInvalidArgs&);
void transtale_connection_problem_error(const ExConnectionProblem&);
void transtale_socket_problem_error(const ExSocketProblem&);

BOOST_PYTHON_MODULE(libfrtpsrv)
{
	class_<CClient>("LibClient", init<std::string, int>(args("server", "port")))
		.def("create_context", &CClient::create_context, return_value_policy<reference_existing_object>())
		.def("connect", &CClient::connect, args("context"))
		.staticmethod("connect")
		.def("invoke", &CClient::invoke, args("context", "command"))
		.staticmethod("invoke")
		.def("get_hash", &CClient::get_hash)
		.staticmethod("get_hash")
	;
	class_<CCommandFactory, boost::noncopyable>("LibCommandFactory", no_init)
		.def("create_command", &CCommandFactory::create, args("cmd_id", "args"), return_value_policy<reference_existing_object>())
		.staticmethod("create_command")
	;
	class_<CContext, boost::noncopyable>("LibContext", no_init);

	register_exception_translator<ExError>(translate_error);
	register_exception_translator<ExInvalidArgs>(transtale_invalid_args_error);
	register_exception_translator<ExConnectionProblem>(transtale_connection_problem_error);
	register_exception_translator<ExSocketProblem>(transtale_socket_problem_error);
}

void translate_error(const ExError& e) {
	PyErr_SetString(PyExc_Exception, e.what());
}

void transtale_invalid_args_error(const ExInvalidArgs& e) {
	PyErr_SetString(PyExc_RuntimeError, e.what());
}

void transtale_connection_problem_error(const ExConnectionProblem& e) {
	PyErr_SetString(PyExc_RuntimeError, e.what());
}

void transtale_socket_problem_error(const ExSocketProblem& e) {
	PyErr_SetString(PyExc_RuntimeError, e.what());
}
