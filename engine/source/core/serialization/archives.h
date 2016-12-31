#pragma once

//#include "cereal/archives/xml.hpp"
// namespace cereal
// {
// 	using OArchive = XMLOutputArchive;
// 	using IArchive = XMLInputArchive;
// }

#include "cereal/archives/json.hpp"
namespace cereal
{
	using OArchive_JSON = JSONOutputArchive;
	using IArchive_JSON = JSONInputArchive;
}

#include "cereal/archives/binary.hpp"
namespace cereal
{
	using OArchive_Binary = BinaryOutputArchive;
	using IArchive_Binary = BinaryInputArchive;
}
// 
// #include "cereal/archives/portable_binary.hpp"
// namespace cereal
// {
// 	using OArchive = PortableBinaryOutputArchive;
// 	using IArchive = PortableBinaryInputArchive;
// }
#define REGISTER_TYPE CEREAL_REGISTER_TYPE