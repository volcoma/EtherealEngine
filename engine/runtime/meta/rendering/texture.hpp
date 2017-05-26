#pragma once
#include "core/reflection/reflection.h"
#include "../../rendering/texture.h"

REFLECT(gfx::TextureInfo)
{
	rttr::registration::enumeration<gfx::TextureFormat::Enum>("TextureFormat")
		(
			rttr::value("BC1", gfx::TextureFormat::BC1),
			rttr::value("BC2", gfx::TextureFormat::BC2),
			rttr::value("BC3", gfx::TextureFormat::BC3),
			rttr::value("BC4", gfx::TextureFormat::BC4),
			rttr::value("BC5", gfx::TextureFormat::BC5),
			rttr::value("BC6H", gfx::TextureFormat::BC6H),
			rttr::value("BC7", gfx::TextureFormat::BC7),
			rttr::value("ETC1", gfx::TextureFormat::ETC1),
			rttr::value("ETC2", gfx::TextureFormat::ETC2),
			rttr::value("ETC2A", gfx::TextureFormat::ETC2A),
			rttr::value("ETC2A1", gfx::TextureFormat::ETC2A1),
			rttr::value("PTC12", gfx::TextureFormat::PTC12),
			rttr::value("PTC14", gfx::TextureFormat::PTC14),
			rttr::value("PTC12A", gfx::TextureFormat::PTC12A),
			rttr::value("PTC14A", gfx::TextureFormat::PTC14A),
			rttr::value("PTC22", gfx::TextureFormat::PTC22),
			rttr::value("PTC24", gfx::TextureFormat::PTC24),
			rttr::value("Unknown", gfx::TextureFormat::Unknown),
			rttr::value("R1", gfx::TextureFormat::R1),
			rttr::value("A8", gfx::TextureFormat::A8),
			rttr::value("R8", gfx::TextureFormat::R8),
			rttr::value("R8I", gfx::TextureFormat::R8I),
			rttr::value("R8U", gfx::TextureFormat::R8U),
			rttr::value("R8S", gfx::TextureFormat::R8S),
			rttr::value("R16", gfx::TextureFormat::R16),
			rttr::value("R16I", gfx::TextureFormat::R16I),
			rttr::value("R16U", gfx::TextureFormat::R16U),
			rttr::value("R16F", gfx::TextureFormat::R16F),
			rttr::value("R16S", gfx::TextureFormat::R16S),
			rttr::value("R32I", gfx::TextureFormat::R32I),
			rttr::value("R32U", gfx::TextureFormat::R32U),
			rttr::value("R32F", gfx::TextureFormat::R32F),
			rttr::value("RG8", gfx::TextureFormat::RG8),
			rttr::value("RG8I", gfx::TextureFormat::RG8I),
			rttr::value("RG8U", gfx::TextureFormat::RG8U),
			rttr::value("RG8S", gfx::TextureFormat::RG8S),
			rttr::value("RG16", gfx::TextureFormat::RG16),
			rttr::value("RG16I", gfx::TextureFormat::RG16I),
			rttr::value("RG16U", gfx::TextureFormat::RG16U),
			rttr::value("RG16F", gfx::TextureFormat::RG16F),
			rttr::value("RG16S", gfx::TextureFormat::RG16S),
			rttr::value("RG32I", gfx::TextureFormat::RG32I),
			rttr::value("RG32U", gfx::TextureFormat::RG32U),
			rttr::value("RG32F", gfx::TextureFormat::RG32F),
			rttr::value("RGB8", gfx::TextureFormat::RGB8),
			rttr::value("RGB8I", gfx::TextureFormat::RGB8I),
			rttr::value("RGB8U", gfx::TextureFormat::RGB8U),
			rttr::value("RGB8S", gfx::TextureFormat::RGB8S),
			rttr::value("RGB9E5F", gfx::TextureFormat::RGB9E5F),
			rttr::value("BGRA8", gfx::TextureFormat::BGRA8),
			rttr::value("RGBA8", gfx::TextureFormat::RGBA8),
			rttr::value("RGBA8I", gfx::TextureFormat::RGBA8I),
			rttr::value("RGBA8U", gfx::TextureFormat::RGBA8U),
			rttr::value("RGBA8S", gfx::TextureFormat::RGBA8S),
			rttr::value("RGBA16", gfx::TextureFormat::RGBA16),
			rttr::value("RGBA16I", gfx::TextureFormat::RGBA16I),
			rttr::value("RGBA16U", gfx::TextureFormat::RGBA16U),
			rttr::value("RGBA16F", gfx::TextureFormat::RGBA16F),
			rttr::value("RGBA16S", gfx::TextureFormat::RGBA16S),
			rttr::value("RGBA32I", gfx::TextureFormat::RGBA32I),
			rttr::value("RGBA32U", gfx::TextureFormat::RGBA32U),
			rttr::value("RGBA32F", gfx::TextureFormat::RGBA32F),
			rttr::value("R5G6B5", gfx::TextureFormat::R5G6B5),
			rttr::value("RGBA4", gfx::TextureFormat::RGBA4),
			rttr::value("RGB5A1", gfx::TextureFormat::RGB5A1),
			rttr::value("RGB10A2", gfx::TextureFormat::RGB10A2),
			rttr::value("RG11B10F", gfx::TextureFormat::RG11B10F),
			rttr::value("UnknownDepth", gfx::TextureFormat::UnknownDepth),
			rttr::value("D16", gfx::TextureFormat::D16),
			rttr::value("D24", gfx::TextureFormat::D24),
			rttr::value("D24S8", gfx::TextureFormat::D24S8),
			rttr::value("D16F", gfx::TextureFormat::D16F),
			rttr::value("D24F", gfx::TextureFormat::D24F),
			rttr::value("D32F", gfx::TextureFormat::D32F),
			rttr::value("D0S8", gfx::TextureFormat::D0S8),
			rttr::value("BC7", gfx::TextureFormat::Count)
			);

	rttr::registration::class_<gfx::TextureInfo>("TextureInfo")
		.property_readonly("format",
			&gfx::TextureInfo::format)
		(
			rttr::metadata("pretty_name", "Format")
		)
		.property_readonly("storageSize",
			&gfx::TextureInfo::storageSize)
		(
			rttr::metadata("pretty_name", "Storage Size")
		)
		.property_readonly("width",
			&gfx::TextureInfo::width)
		(
			rttr::metadata("pretty_name", "Width")
		)
		.property_readonly("height",
			&gfx::TextureInfo::height)
		(
			rttr::metadata("pretty_name", "Height")
		)
		.property_readonly("depth",
			&gfx::TextureInfo::depth)
		(
			rttr::metadata("pretty_name", "Depth")
		)
		.property_readonly("numMips",
			&gfx::TextureInfo::numMips)
		(
			rttr::metadata("pretty_name", "Mips")
		)
		.property_readonly("bitsPerPixel",
			&gfx::TextureInfo::bitsPerPixel)
		(
			rttr::metadata("pretty_name", "Bits Per Pixel")
		)
		.property_readonly("cubeMap",
			&gfx::TextureInfo::cubeMap)
		(
			rttr::metadata("pretty_name", "Cubemap")
		)
		;
}
