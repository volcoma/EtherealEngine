/*
 * Copyright 2011-2016 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

#ifndef SHADERC_H_HEADER_GUARD
#define SHADERC_H_HEADER_GUARD

#ifndef SHADERC_CONFIG_HLSL
#	define SHADERC_CONFIG_HLSL BX_PLATFORM_WINDOWS
#endif // SHADERC_CONFIG_HLSL

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <string>
#include <vector>
#include <unordered_map>

#include "Graphics/bx/bx.h"
#include "Graphics/bx/debug.h"
#include "Graphics/bx/commandline.h"
#include "Graphics/bx/endian.h"
#include "Graphics/bx/uint32_t.h"
#include "Graphics/bx/string.h"
#include "Graphics/bx/hash.h"
#include "Graphics/bx/crtimpl.h"
#include "Graphics/src/vertexdecl.h"

namespace bgfx
{
	extern bool g_verbose;

	class LineReader
	{
	public:
		LineReader(const char* _str)
			: m_str(_str)
			, m_pos(0)
			, m_size((uint32_t)strlen(_str))
		{
		}

		std::string getLine()
		{
			const char* str = &m_str[m_pos];
			skipLine();

			const char* eol = &m_str[m_pos];

			std::string tmp;
			tmp.assign(str, eol - str);
			return tmp;
		}

		bool isEof() const
		{
			return m_str[m_pos] == '\0';
		}

		void skipLine()
		{
			const char* str = &m_str[m_pos];
			const char* nl = bx::strnl(str);
			m_pos += (uint32_t)(nl - str);
		}

		const char* m_str;
		uint32_t m_pos;
		uint32_t m_size;
	};

#define BGFX_UNIFORM_FRAGMENTBIT UINT8_C(0x10)
#define BGFX_UNIFORM_SAMPLERBIT  UINT8_C(0x20)

	const char* getUniformTypeName_(UniformType::Enum _enum);
	UniformType::Enum nameToUniformTypeEnum_(const char* _name);

	struct Uniform
	{
		std::string name;
		UniformType::Enum type;
		uint8_t num;
		uint16_t regIndex;
		uint16_t regCount;
	};

	typedef std::vector<Uniform> UniformArray;

	void printCode(const char* _code, int32_t _line = 0, int32_t _start = 0, int32_t _end = INT32_MAX, int32_t _column = -1);
	void strReplace(char* _str, const char* _find, const char* _replace);
	int32_t writef(bx::WriterI* _writer, const char* _format, ...);
	void writeFile(const char* _filePath, const void* _data, int32_t _size);

	bool compileGLSLShader(bx::CommandLine& _cmdLine, uint32_t _version, const std::string& _code, bx::WriterI* _writer);
	bool compileHLSLShader(bx::CommandLine& _cmdLine, uint32_t _version, const std::string& _code, bx::WriterI* _writer);
	bool compilePSSLShader(bx::CommandLine& _cmdLine, uint32_t _version, const std::string& _code, bx::WriterI* _writer);
	bool compileSPIRVShader(bx::CommandLine& _cmdLine, uint32_t _version, const std::string& _code, bx::WriterI* _writer);

} // namespace bgfx

int compileShader(int _argc, const char* _argv[]);

#endif // SHADERC_H_HEADER_GUARD
