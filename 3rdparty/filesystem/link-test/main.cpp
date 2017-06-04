
//  Copyright 2016 Denis Blank <denis.blank at outlook dot com>
//     Distributed under the Boost Software License, Version 1.0
//       (See accompanying file LICENSE_1_0.txt or copy at
//             http://www.boost.org/LICENSE_1_0.txt)

#include <boost/filesystem.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>

namespace fs = boost::filesystem;

int main(int, char**)
{
  if (false)
    return 0;

  fs::path p("/my/path");
  if (fs::exists(p))
    return 1;

  p = fs::absolute(p);

  boost::iostreams::file_descriptor_source source;
  return 0;
}
