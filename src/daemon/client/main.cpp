#include <boost/python.hpp>

#include "cclient.h"

void translate_error(const ExError&);
void transtale_invalid_args_error(const ExInvalidArgs&);
void transtale_connection_problem_error(const ExConnectionProblem&);
void transtale_socket_problem_error(const ExSocketProblem&);
void translate_unknown_data_type_error(const ExUnknownDataType&);
void translate_no_file_error(const ExNoFile&);

BOOST_PYTHON_MODULE(libfrtpsrv)
{
	boost::python::class_<CClient>("LibClient",
				       boost::python::init<std::string, std::string>(boost::python::args("workingdir",
													 "datasubdir")))
		.def("connect", &CClient::connect, boost::python::args("server", "port", "login", "password"))
		.def("invoke", &CClient::invoke, boost::python::args("command", "datatype"))
		.def("get_hash", &CClient::get_hash)
		.staticmethod("get_hash")
		.def("check_integrity", &CClient::check_integrity, boost::python::args("server", "client", "datatype"))
	;
	boost::python::class_<utility::CCommandFactory, boost::noncopyable>("LibCommandFactory", boost::python::no_init)
		.def("create_command", static_cast<utility::ICommand* (*)(
					const std::string&,
					boost::python::list)>(&utility::CCommandFactory::create),
		     boost::python::return_value_policy<boost::python::manage_new_object>())
		.def("create_command", static_cast<utility::ICommand* (*)(
					const utility::CMessage&)>(&utility::CCommandFactory::create),
		     boost::python::return_value_policy<boost::python::manage_new_object>())
		.staticmethod("create_command")
	;
	boost::python::class_<utility::ICommand, boost::noncopyable>("LibCommand", boost::python::no_init);
	boost::python::class_<utility::CMessage, boost::noncopyable>("LibMessage", boost::python::no_init);

	boost::python::register_exception_translator<ExError>(translate_error);
	boost::python::register_exception_translator<ExInvalidArgs>(transtale_invalid_args_error);
	boost::python::register_exception_translator<ExConnectionProblem>(transtale_connection_problem_error);
	boost::python::register_exception_translator<ExSocketProblem>(transtale_socket_problem_error);
	boost::python::register_exception_translator<ExUnknownDataType>(translate_unknown_data_type_error);
	boost::python::register_exception_translator<ExNoFile>(translate_no_file_error);
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
	PyErr_SetString(PyExc_TypeError, e.what());
}

void translate_no_file_error(const ExNoFile& e) {
	PyErr_SetString(PyExc_RuntimeError, e.what());
}
