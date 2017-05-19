#pragma once

// #include "cereal/archives/xml.hpp"
// namespace cereal
// {
// 	using oarchive_xml_t = XMLOutputArchive;
// 	using iarchive_xml_t = XMLInputArchive;
// }

#include "cereal/archives/json.hpp"
namespace cereal
{
	using oarchive_json_t = JSONOutputArchive;
	using iarchive_json_t = JSONInputArchive;
}

#include "cereal/archives/binary.hpp"
namespace cereal
{
	using oarchive_binary_t = BinaryOutputArchive;
	using iarchive_binary_t = BinaryInputArchive;
}

// #include "cereal/archives/portable_binary.hpp"
// namespace cereal
// {
// 	using oarchive_portable_binary_t = PortableBinaryOutputArchive;
// 	using iarchive_portable_binary_t = PortableBinaryInputArchive;
// }