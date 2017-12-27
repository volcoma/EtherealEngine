#include "texture.hpp"

namespace gfx
{
REFLECT(texture_info)
{
	rttr::registration::enumeration<texture_format>("texture_format")(
		rttr::value("BC1", texture_format::BC1), rttr::value("BC2", texture_format::BC2),
		rttr::value("BC3", texture_format::BC3), rttr::value("BC4", texture_format::BC4),
		rttr::value("BC5", texture_format::BC5), rttr::value("BC6H", texture_format::BC6H),
		rttr::value("BC7", texture_format::BC7), rttr::value("ETC1", texture_format::ETC1),
		rttr::value("ETC2", texture_format::ETC2), rttr::value("ETC2A", texture_format::ETC2A),
		rttr::value("ETC2A1", texture_format::ETC2A1), rttr::value("PTC12", texture_format::PTC12),
		rttr::value("PTC14", texture_format::PTC14), rttr::value("PTC12A", texture_format::PTC12A),
		rttr::value("PTC14A", texture_format::PTC14A), rttr::value("PTC22", texture_format::PTC22),
		rttr::value("PTC24", texture_format::PTC24), rttr::value("Unknown", texture_format::Unknown),
		rttr::value("R1", texture_format::R1), rttr::value("A8", texture_format::A8),
		rttr::value("R8", texture_format::R8), rttr::value("R8I", texture_format::R8I),
		rttr::value("R8U", texture_format::R8U), rttr::value("R8S", texture_format::R8S),
		rttr::value("R16", texture_format::R16), rttr::value("R16I", texture_format::R16I),
		rttr::value("R16U", texture_format::R16U), rttr::value("R16F", texture_format::R16F),
		rttr::value("R16S", texture_format::R16S), rttr::value("R32I", texture_format::R32I),
		rttr::value("R32U", texture_format::R32U), rttr::value("R32F", texture_format::R32F),
		rttr::value("RG8", texture_format::RG8), rttr::value("RG8I", texture_format::RG8I),
		rttr::value("RG8U", texture_format::RG8U), rttr::value("RG8S", texture_format::RG8S),
		rttr::value("RG16", texture_format::RG16), rttr::value("RG16I", texture_format::RG16I),
		rttr::value("RG16U", texture_format::RG16U), rttr::value("RG16F", texture_format::RG16F),
		rttr::value("RG16S", texture_format::RG16S), rttr::value("RG32I", texture_format::RG32I),
		rttr::value("RG32U", texture_format::RG32U), rttr::value("RG32F", texture_format::RG32F),
		rttr::value("RGB8", texture_format::RGB8), rttr::value("RGB8I", texture_format::RGB8I),
		rttr::value("RGB8U", texture_format::RGB8U), rttr::value("RGB8S", texture_format::RGB8S),
		rttr::value("RGB9E5F", texture_format::RGB9E5F), rttr::value("BGRA8", texture_format::BGRA8),
		rttr::value("RGBA8", texture_format::RGBA8), rttr::value("RGBA8I", texture_format::RGBA8I),
		rttr::value("RGBA8U", texture_format::RGBA8U), rttr::value("RGBA8S", texture_format::RGBA8S),
		rttr::value("RGBA16", texture_format::RGBA16), rttr::value("RGBA16I", texture_format::RGBA16I),
		rttr::value("RGBA16U", texture_format::RGBA16U), rttr::value("RGBA16F", texture_format::RGBA16F),
		rttr::value("RGBA16S", texture_format::RGBA16S), rttr::value("RGBA32I", texture_format::RGBA32I),
		rttr::value("RGBA32U", texture_format::RGBA32U), rttr::value("RGBA32F", texture_format::RGBA32F),
		rttr::value("R5G6B5", texture_format::R5G6B5), rttr::value("RGBA4", texture_format::RGBA4),
		rttr::value("RGB5A1", texture_format::RGB5A1), rttr::value("RGB10A2", texture_format::RGB10A2),
		rttr::value("RG11B10F", texture_format::RG11B10F),
		rttr::value("UnknownDepth", texture_format::UnknownDepth), rttr::value("D16", texture_format::D16),
		rttr::value("D24", texture_format::D24), rttr::value("D24S8", texture_format::D24S8),
		rttr::value("D16F", texture_format::D16F), rttr::value("D24F", texture_format::D24F),
		rttr::value("D32F", texture_format::D32F), rttr::value("D0S8", texture_format::D0S8),
		rttr::value("BC7", texture_format::Count));

	rttr::registration::class_<texture_info>("texture_info")
		.property_readonly("format", &texture_info::format)(rttr::metadata("pretty_name", "Format"))
		.property_readonly("storageSize",
						   &texture_info::storageSize)(rttr::metadata("pretty_name", "Storage Size(bytes)"))
		.property_readonly("width", &texture_info::width)(rttr::metadata("pretty_name", "Width"))
		.property_readonly("height", &texture_info::height)(rttr::metadata("pretty_name", "Height"))
		.property_readonly("depth", &texture_info::depth)(rttr::metadata("pretty_name", "Depth"))
		.property_readonly("numMips", &texture_info::numMips)(rttr::metadata("pretty_name", "Mips"))
		.property_readonly("bitsPerPixel",
						   &texture_info::bitsPerPixel)(rttr::metadata("pretty_name", "Bits Per Pixel"))
		.property_readonly("cubeMap", &texture_info::cubeMap)(rttr::metadata("pretty_name", "Cubemap"));
}
}
