#pragma once

//#include "cereal/archives/portable_binary.hpp"
//namespace cereal
//{
//    using oarchive_binary_t = PortableBinaryOutputArchive;
//    using iarchive_binary_t = PortableBinaryInputArchive;
//}

#include "cereal/archives/binary.hpp"
namespace cereal
{
    using oarchive_binary_t = BinaryOutputArchive;
    using iarchive_binary_t = BinaryInputArchive;
}
