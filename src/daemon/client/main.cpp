#include <boost/python.hpp>

#include "cclient.h"

using namespace boost::python;

void translate_error(const ExError&);
void transtale_invalid_args_error(const ExInvalidArgs&);
void transtale_connection_problem_error(const ExConnectionProblem&);
void transtale_socket_problem_error(const ExSocketProblem&);
void translate_unknown_data_type_error(const ExUnknownDataType&);
void translate_no_file_error(const ExNoFile&);

BOOST_PYTHON_MODULE(libfrtpsrv)
{
	class_<CClient>("LibClient", init<std::string>(args("workingdir")))
		.def("create_context", &CClient::create_context, return_value_policy<manage_new_object>())
		.def("connect", &CClient::connect, args("context", "server", "port"))
		.staticmethod("connect")
		.def("invoke", &CClient::invoke, args("context", "command", "datatype"))
		.staticmethod("invoke")
		.def("get_hash", &CClient::get_hash)
		.staticmethod("get_hash")
		.def("check_integrity", &CClient::check_integrity, args("context", "server", "client", "datatype"))
		.staticmethod("check_integrity")
	;
	class_<CCommandFactory, boost::noncopyable>("LibCommandFactory", no_init)
		.def("create_command", static_cast<ICommand* (*)(const std::string&, boost::python::list)>(&CCommandFactory::create),
		     return_value_policy<manage_new_object>())
		.def("create_command", static_cast<ICommand* (*)(const CMessage&)>(&CCommandFactory::create),
		     return_value_policy<manage_new_object>())
		.staticmethod("create_command")
	;
	class_<CContext, boost::noncopyable>("LibContext", no_init);
	class_<ICommand, boost::noncopyable>("LibCommand", no_init);
	class_<CMessage, boost::noncopyable>("LibMessage", no_init);

	register_exception_translator<ExError>(translate_error);
	register_exception_translator<ExInvalidArgs>(transtale_invalid_args_error);
	register_exception_translator<ExConnectionProblem>(transtale_connection_problem_error);
	register_exception_translator<ExSocketProblem>(transtale_socket_problem_error);
	register_exception_translator<ExUnknownDataType>(translate_unknown_data_type_error);
	register_exception_translator<ExNoFile>(translate_no_file_error);
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

void translate_unknown_data_type_error(const ExUnknownDataType& e) {
	PyErr_SetString(PyExc_RuntimeError, e.what());
}

void translate_no_file_error(const ExNoFile& e) {
	PyErr_SetString(PyExc_RuntimeError, e.what());
}
