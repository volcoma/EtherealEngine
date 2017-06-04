#pragma once

// #include "cereal/archives/xml.hpp"
// namespace cereal
// {
// 	using oarchive_associative_t = XMLOutputArchive;
// 	using iarchive_associative_t = XMLInputArchive;
// }

#include "cereal/archives/json.hpp"
namespace cereal
{
    using oarchive_associative_t = JSONOutputArchive;
    using iarchive_associative_t = JSONInputArchive;
}
