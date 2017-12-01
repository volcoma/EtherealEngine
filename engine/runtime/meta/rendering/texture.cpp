#include "texture.hpp"

REFLECT(gfx::texture_info)
{
	rttr::registration::enumeration<gfx::texture_format>("texture_format")(
		rttr::value("BC1", gfx::texture_format::BC1), rttr::value("BC2", gfx::texture_format::BC2),
		rttr::value("BC3", gfx::texture_format::BC3), rttr::value("BC4", gfx::texture_format::BC4),
		rttr::value("BC5", gfx::texture_format::BC5), rttr::value("BC6H", gfx::texture_format::BC6H),
		rttr::value("BC7", gfx::texture_format::BC7), rttr::value("ETC1", gfx::texture_format::ETC1),
		rttr::value("ETC2", gfx::texture_format::ETC2), rttr::value("ETC2A", gfx::texture_format::ETC2A),
		rttr::value("ETC2A1", gfx::texture_format::ETC2A1), rttr::value("PTC12", gfx::texture_format::PTC12),
		rttr::value("PTC14", gfx::texture_format::PTC14), rttr::value("PTC12A", gfx::texture_format::PTC12A),
		rttr::value("PTC14A", gfx::texture_format::PTC14A), rttr::value("PTC22", gfx::texture_format::PTC22),
		rttr::value("PTC24", gfx::texture_format::PTC24),
		rttr::value("Unknown", gfx::texture_format::Unknown), rttr::value("R1", gfx::texture_format::R1),
		rttr::value("A8", gfx::texture_format::A8), rttr::value("R8", gfx::texture_format::R8),
		rttr::value("R8I", gfx::texture_format::R8I), rttr::value("R8U", gfx::texture_format::R8U),
		rttr::value("R8S", gfx::texture_format::R8S), rttr::value("R16", gfx::texture_format::R16),
		rttr::value("R16I", gfx::texture_format::R16I), rttr::value("R16U", gfx::texture_format::R16U),
		rttr::value("R16F", gfx::texture_format::R16F), rttr::value("R16S", gfx::texture_format::R16S),
		rttr::value("R32I", gfx::texture_format::R32I), rttr::value("R32U", gfx::texture_format::R32U),
		rttr::value("R32F", gfx::texture_format::R32F), rttr::value("RG8", gfx::texture_format::RG8),
		rttr::value("RG8I", gfx::texture_format::RG8I), rttr::value("RG8U", gfx::texture_format::RG8U),
		rttr::value("RG8S", gfx::texture_format::RG8S), rttr::value("RG16", gfx::texture_format::RG16),
		rttr::value("RG16I", gfx::texture_format::RG16I), rttr::value("RG16U", gfx::texture_format::RG16U),
		rttr::value("RG16F", gfx::texture_format::RG16F), rttr::value("RG16S", gfx::texture_format::RG16S),
		rttr::value("RG32I", gfx::texture_format::RG32I), rttr::value("RG32U", gfx::texture_format::RG32U),
		rttr::value("RG32F", gfx::texture_format::RG32F), rttr::value("RGB8", gfx::texture_format::RGB8),
		rttr::value("RGB8I", gfx::texture_format::RGB8I), rttr::value("RGB8U", gfx::texture_format::RGB8U),
		rttr::value("RGB8S", gfx::texture_format::RGB8S),
		rttr::value("RGB9E5F", gfx::texture_format::RGB9E5F),
		rttr::value("BGRA8", gfx::texture_format::BGRA8), rttr::value("RGBA8", gfx::texture_format::RGBA8),
		rttr::value("RGBA8I", gfx::texture_format::RGBA8I),
		rttr::value("RGBA8U", gfx::texture_format::RGBA8U),
		rttr::value("RGBA8S", gfx::texture_format::RGBA8S),
		rttr::value("RGBA16", gfx::texture_format::RGBA16),
		rttr::value("RGBA16I", gfx::texture_format::RGBA16I),
		rttr::value("RGBA16U", gfx::texture_format::RGBA16U),
		rttr::value("RGBA16F", gfx::texture_format::RGBA16F),
		rttr::value("RGBA16S", gfx::texture_format::RGBA16S),
		rttr::value("RGBA32I", gfx::texture_format::RGBA32I),
		rttr::value("RGBA32U", gfx::texture_format::RGBA32U),
		rttr::value("RGBA32F", gfx::texture_format::RGBA32F),
		rttr::value("R5G6B5", gfx::texture_format::R5G6B5), rttr::value("RGBA4", gfx::texture_format::RGBA4),
		rttr::value("RGB5A1", gfx::texture_format::RGB5A1),
		rttr::value("RGB10A2", gfx::texture_format::RGB10A2),
		rttr::value("RG11B10F", gfx::texture_format::RG11B10F),
		rttr::value("UnknownDepth", gfx::texture_format::UnknownDepth),
		rttr::value("D16", gfx::texture_format::D16), rttr::value("D24", gfx::texture_format::D24),
		rttr::value("D24S8", gfx::texture_format::D24S8), rttr::value("D16F", gfx::texture_format::D16F),
		rttr::value("D24F", gfx::texture_format::D24F), rttr::value("D32F", gfx::texture_format::D32F),
		rttr::value("D0S8", gfx::texture_format::D0S8), rttr::value("BC7", gfx::texture_format::Count));

	rttr::registration::class_<gfx::texture_info>("texture_info")
		.property_readonly("format", &gfx::texture_info::format)(rttr::metadata("pretty_name", "Format"))
		.property_readonly("storageSize",
						   &gfx::texture_info::storageSize)(rttr::metadata("pretty_name", "Storage Size"))
		.property_readonly("width", &gfx::texture_info::width)(rttr::metadata("pretty_name", "Width"))
		.property_readonly("height", &gfx::texture_info::height)(rttr::metadata("pretty_name", "Height"))
		.property_readonly("depth", &gfx::texture_info::depth)(rttr::metadata("pretty_name", "Depth"))
		.property_readonly("numMips", &gfx::texture_info::numMips)(rttr::metadata("pretty_name", "Mips"))
		.property_readonly("bitsPerPixel",
						   &gfx::texture_info::bitsPerPixel)(rttr::metadata("pretty_name", "Bits Per Pixel"))
		.property_readonly("cubeMap", &gfx::texture_info::cubeMap)(rttr::metadata("pretty_name", "Cubemap"));
}
