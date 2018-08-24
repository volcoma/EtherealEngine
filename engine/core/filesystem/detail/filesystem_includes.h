#pragma once

#include "boost/filesystem.hpp"
#include <chrono>

//#include <experimental/filesystem>

namespace fs
{
//    using namespace std::experimental::filesystem;
//    using error_code = std::error_code;
//    using file_time_type = std::experimental::filesystem::file_time_type;
//    using copy_options = std::experimental::filesystem::copy_options;
//    inline file_time_type now()
//    {
//        return std::chrono::system_clock::now();
//    }

using namespace boost::filesystem;
using error_code = boost::system::error_code;
using file_time_type = std::time_t;
using copy_options = boost::filesystem::copy_option;
inline file_time_type now()
{
	return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}
}
