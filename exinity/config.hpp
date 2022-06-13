#pragma once 
#include <iostream>
#include <boost/multiprecision/cpp_dec_float.hpp>

#define DEBUG 0

extern const std::string basePath = "";

extern const std::string fErrLog = "PassivePeg_errors.log";
extern const std::string fLog = "PassivePeg.log";

extern const std::string pLog = "ProfileExec_for_debug.log";

typedef boost::multiprecision::cpp_dec_float_50 decimal;