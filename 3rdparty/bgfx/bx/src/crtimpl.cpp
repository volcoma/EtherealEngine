/*
 * Copyright 2010-2017 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx#license-bsd-2-clause
 */

#include <bx/crtimpl.h>

#include <stdio.h>
#include <malloc.h>

#ifndef BX_CONFIG_CRT_FILE_READER_WRITER
#	define BX_CONFIG_CRT_FILE_READER_WRITER !(0 \
			|| BX_CRT_NONE                      \
			)
#endif // BX_CONFIG_CRT_FILE_READER_WRITER

#ifndef BX_CONFIG_CRT_PROCESS
#	define BX_CONFIG_CRT_PROCESS !(0  \
			|| BX_CRT_NONE            \
			|| BX_PLATFORM_EMSCRIPTEN \
			|| BX_PLATFORM_PS4        \
			|| BX_PLATFORM_WINRT      \
			|| BX_PLATFORM_XBOXONE    \
			)
#endif // BX_CONFIG_CRT_PROCESS

namespace bx
{
	DefaultAllocator::DefaultAllocator()
	{
	}

	DefaultAllocator::~DefaultAllocator()
	{
	}

	void* DefaultAllocator::realloc(void* _ptr, size_t _size, size_t _align, const char* _file, uint32_t _line)
	{
		if (0 == _size)
		{
			if (NULL != _ptr)
			{
				if (BX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT >= _align)
				{
					::free(_ptr);
					return NULL;
				}

#	if BX_COMPILER_MSVC
				BX_UNUSED(_file, _line);
				_aligned_free(_ptr);
#	else
				bx::alignedFree(this, _ptr, _align, _file, _line);
#	endif // BX_
			}

			return NULL;
		}
		else if (NULL == _ptr)
		{
			if (BX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT >= _align)
			{
				return ::malloc(_size);
			}

#	if BX_COMPILER_MSVC
			BX_UNUSED(_file, _line);
			return _aligned_malloc(_size, _align);
#	else
			return bx::alignedAlloc(this, _size, _align, _file, _line);
#	endif // BX_
		}

		if (BX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT >= _align)
		{
			return ::realloc(_ptr, _size);
		}

#	if BX_COMPILER_MSVC
		BX_UNUSED(_file, _line);
		return _aligned_realloc(_ptr, _size, _align);
#	else
		return bx::alignedRealloc(this, _ptr, _size, _align, _file, _line);
#	endif // BX_
	}

#if BX_CONFIG_CRT_FILE_READER_WRITER

#	if BX_CRT_MSVC
#		define fseeko64 _fseeki64
#		define ftello64 _ftelli64
#	elif 0 \
	  || BX_PLATFORM_ANDROID \
	  || BX_PLATFORM_BSD \
	  || BX_PLATFORM_IOS \
	  || BX_PLATFORM_OSX \
	  || BX_PLATFORM_QNX
#		define fseeko64 fseeko
#		define ftello64 ftello
#	elif BX_PLATFORM_PS4
#		define fseeko64 fseek
#		define ftello64 ftell
#	endif // BX_

	class FileReaderImpl : public bx::FileReaderI
	{
	public:
		FileReaderImpl(FILE* _file)
			: m_file(_file)
			, m_open(false)
		{
		}

		virtual ~FileReaderImpl()
		{
			close();
		}

		virtual bool open(const char* _filePath, Error* _err) BX_OVERRIDE
		{
			BX_CHECK(NULL != _err, "Reader/Writer interface calling functions must handle errors.");

			if (NULL != m_file)
			{
				BX_ERROR_SET(_err, BX_ERROR_READERWRITER_ALREADY_OPEN, "FileReader: File is already open.");
				return false;
			}

			m_file = fopen(_filePath, "rb");
			if (NULL == m_file)
			{
				BX_ERROR_SET(_err, BX_ERROR_READERWRITER_OPEN, "FileReader: Failed to open file.");
				return false;
			}

			m_open = true;
			return true;
		}

		virtual void close() BX_OVERRIDE
		{
			if (m_open
			&&  NULL != m_file)
			{
				fclose(m_file);
				m_file = NULL;
			}
		}

		virtual int64_t seek(int64_t _offset, Whence::Enum _whence) BX_OVERRIDE
		{
			BX_CHECK(NULL != m_file, "Reader/Writer file is not open.");
			fseeko64(m_file, _offset, _whence);
			return ftello64(m_file);
		}

		virtual int32_t read(void* _data, int32_t _size, Error* _err) BX_OVERRIDE
		{
			BX_CHECK(NULL != m_file, "Reader/Writer file is not open.");
			BX_CHECK(NULL != _err, "Reader/Writer interface calling functions must handle errors.");

			int32_t size = (int32_t)fread(_data, 1, _size, m_file);
			if (size != _size)
			{
				if (0 != feof(m_file) )
				{
					BX_ERROR_SET(_err, BX_ERROR_READERWRITER_EOF, "FileReader: EOF.");
				}
				else if (0 != ferror(m_file) )
				{
					BX_ERROR_SET(_err, BX_ERROR_READERWRITER_READ, "FileReader: read error.");
				}

				return size >= 0 ? size : 0;
			}

			return size;
		}

	private:
		FILE* m_file;
		bool  m_open;
	};

	class FileWriterImpl : public bx::FileWriterI
	{
	public:
		FileWriterImpl(FILE* _file)
			: m_file(_file)
			, m_open(false)
		{
		}

		virtual ~FileWriterImpl()
		{
			close();
		}

		virtual bool open(const char* _filePath, bool _append, Error* _err) BX_OVERRIDE
		{
			BX_CHECK(NULL != _err, "Reader/Writer interface calling functions must handle errors.");

			if (NULL != m_file)
			{
				BX_ERROR_SET(_err, BX_ERROR_READERWRITER_ALREADY_OPEN, "FileReader: File is already open.");
				return false;
			}

			m_file = fopen(_filePath, _append ? "ab" : "wb");

			if (NULL == m_file)
			{
				BX_ERROR_SET(_err, BX_ERROR_READERWRITER_OPEN, "FileWriter: Failed to open file.");
				return false;
			}

			m_open = true;
			return true;
		}

		virtual void close() BX_OVERRIDE
		{
			if (m_open
			&&  NULL != m_file)
			{
				fclose(m_file);
				m_file = NULL;
			}
		}

		virtual int64_t seek(int64_t _offset, Whence::Enum _whence) BX_OVERRIDE
		{
			BX_CHECK(NULL != m_file, "Reader/Writer file is not open.");
			fseeko64(m_file, _offset, _whence);
			return ftello64(m_file);
		}

		virtual int32_t write(const void* _data, int32_t _size, Error* _err) BX_OVERRIDE
		{
			BX_CHECK(NULL != m_file, "Reader/Writer file is not open.");
			BX_CHECK(NULL != _err, "Reader/Writer interface calling functions must handle errors.");

			int32_t size = (int32_t)fwrite(_data, 1, _size, m_file);
			if (size != _size)
			{
				BX_ERROR_SET(_err, BX_ERROR_READERWRITER_WRITE, "FileWriter: write failed.");
				return size >= 0 ? size : 0;
			}

			return size;
		}

	private:
		FILE* m_file;
		bool  m_open;
	};

#else

	class FileReaderImpl : public bx::FileReaderI
	{
	public:
		FileReaderImpl(void*)
		{
		}

		virtual ~FileReaderImpl()
		{
			close();
		}

		virtual bool open(const char* _filePath, Error* _err) BX_OVERRIDE
		{
			BX_UNUSED(_filePath, _err);
			return false;
		}

		virtual void close() BX_OVERRIDE
		{
		}

		virtual int64_t seek(int64_t _offset, Whence::Enum _whence) BX_OVERRIDE
		{
			BX_UNUSED(_offset, _whence);
			return 0;
		}

		virtual int32_t read(void* _data, int32_t _size, Error* _err) BX_OVERRIDE
		{
			BX_UNUSED(_data, _size, _err);
			return 0;
		}
	};

	class FileWriterImpl : public bx::FileWriterI
	{
	public:
		FileWriterImpl(void*)
		{
		}

		virtual ~FileWriterImpl()
		{
			close();
		}

		virtual bool open(const char* _filePath, bool _append, Error* _err) BX_OVERRIDE
		{
			BX_UNUSED(_filePath, _append);
			return false;
		}

		virtual void close() BX_OVERRIDE
		{
		}

		virtual int64_t seek(int64_t _offset, Whence::Enum _whence) BX_OVERRIDE
		{
			BX_UNUSED(_offset, _whence);
			return 0;
		}

		virtual int32_t write(const void* _data, int32_t _size, Error* _err) BX_OVERRIDE
		{
			BX_UNUSED(_data, _size, _err);
			return 0;
		}
	};

#endif // BX_CONFIG_CRT_FILE_READER_WRITER

	FileReader::FileReader()
	{
		BX_STATIC_ASSERT(sizeof(FileReaderImpl) <= sizeof(m_internal) );
		new(m_internal) FileReaderImpl(NULL);
	}

	FileReader::~FileReader()
	{
		FileReaderImpl* impl = reinterpret_cast<FileReaderImpl*>(m_internal);
		impl->~FileReaderImpl();
	}

	bool FileReader::open(const char* _filePath, Error* _err)
	{
		FileReaderImpl* impl = reinterpret_cast<FileReaderImpl*>(m_internal);
		return impl->open(_filePath, _err);
	}

	void FileReader::close()
	{
		FileReaderImpl* impl = reinterpret_cast<FileReaderImpl*>(m_internal);
		impl->close();
	}

	int64_t FileReader::seek(int64_t _offset, Whence::Enum _whence)
	{
		FileReaderImpl* impl = reinterpret_cast<FileReaderImpl*>(m_internal);
		return impl->seek(_offset, _whence);
	}

	int32_t FileReader::read(void* _data, int32_t _size, Error* _err)
	{
		FileReaderImpl* impl = reinterpret_cast<FileReaderImpl*>(m_internal);
		return impl->read(_data, _size, _err);
	}

	FileWriter::FileWriter()
	{
		BX_STATIC_ASSERT(sizeof(FileWriterImpl) <= sizeof(m_internal) );
		new(m_internal) FileWriterImpl(NULL);
	}

	FileWriter::~FileWriter()
	{
		FileWriterImpl* impl = reinterpret_cast<FileWriterImpl*>(m_internal);
		impl->~FileWriterImpl();
	}

	bool FileWriter::open(const char* _filePath, bool _append, Error* _err)
	{
		FileWriterImpl* impl = reinterpret_cast<FileWriterImpl*>(m_internal);
		return impl->open(_filePath, _append, _err);
	}

	void FileWriter::close()
	{
		FileWriterImpl* impl = reinterpret_cast<FileWriterImpl*>(m_internal);
		impl->close();
	}

	int64_t FileWriter::seek(int64_t _offset, Whence::Enum _whence)
	{
		FileWriterImpl* impl = reinterpret_cast<FileWriterImpl*>(m_internal);
		return impl->seek(_offset, _whence);
	}

	int32_t FileWriter::write(const void* _data, int32_t _size, Error* _err)
	{
		FileWriterImpl* impl = reinterpret_cast<FileWriterImpl*>(m_internal);
		return impl->write(_data, _size, _err);
	}

	ReaderI* getStdIn()
	{
		static FileReaderImpl s_stdIn(stdout);
		return &s_stdIn;
	}

	WriterI* getStdOut()
	{
		static FileWriterImpl s_stdOut(stdout);
		return &s_stdOut;
	}

	WriterI* getStdErr()
	{
		static FileWriterImpl s_stdOut(stderr);
		return &s_stdOut;
	}

#if BX_CONFIG_CRT_PROCESS

#if BX_CRT_MSVC
#	define popen  _popen
#	define pclose _pclose
#endif // BX_CRT_MSVC

	ProcessReader::ProcessReader()
		: m_file(NULL)
	{
	}

	ProcessReader::~ProcessReader()
	{
		BX_CHECK(NULL == m_file, "Process not closed!");
	}

	bool ProcessReader::open(const char* _command, Error* _err)
	{
		BX_CHECK(NULL != _err, "Reader/Writer interface calling functions must handle errors.");

		if (NULL != m_file)
		{
			BX_ERROR_SET(_err, BX_ERROR_READERWRITER_ALREADY_OPEN, "ProcessReader: File is already open.");
			return false;
		}

		m_file = popen(_command, "r");
		if (NULL == m_file)
		{
			BX_ERROR_SET(_err, BX_ERROR_READERWRITER_OPEN, "ProcessReader: Failed to open process.");
			return false;
		}

		return true;
	}

	void ProcessReader::close()
	{
		BX_CHECK(NULL != m_file, "Process not open!");
		FILE* file = (FILE*)m_file;
		m_exitCode = pclose(file);
		m_file = NULL;
	}

	int32_t ProcessReader::read(void* _data, int32_t _size, Error* _err)
	{
		BX_CHECK(NULL != _err, "Reader/Writer interface calling functions must handle errors."); BX_UNUSED(_err);

		FILE* file = (FILE*)m_file;
		int32_t size = (int32_t)fread(_data, 1, _size, file);
		if (size != _size)
		{
			if (0 != feof(file) )
			{
				BX_ERROR_SET(_err, BX_ERROR_READERWRITER_EOF, "ProcessReader: EOF.");
			}
			else if (0 != ferror(file) )
			{
				BX_ERROR_SET(_err, BX_ERROR_READERWRITER_READ, "ProcessReader: read error.");
			}

			return size >= 0 ? size : 0;
		}

		return size;
	}

	int32_t ProcessReader::getExitCode() const
	{
		return m_exitCode;
	}

	ProcessWriter::ProcessWriter()
		: m_file(NULL)
	{
	}

	ProcessWriter::~ProcessWriter()
	{
		BX_CHECK(NULL == m_file, "Process not closed!");
	}

	bool ProcessWriter::open(const char* _command, bool, Error* _err)
	{
		BX_CHECK(NULL != _err, "Reader/Writer interface calling functions must handle errors.");

		if (NULL != m_file)
		{
			BX_ERROR_SET(_err, BX_ERROR_READERWRITER_ALREADY_OPEN, "ProcessWriter: File is already open.");
			return false;
		}

		m_file = popen(_command, "w");
		if (NULL == m_file)
		{
			BX_ERROR_SET(_err, BX_ERROR_READERWRITER_OPEN, "ProcessWriter: Failed to open process.");
			return false;
		}

		return true;
	}

	void ProcessWriter::close()
	{
		BX_CHECK(NULL != m_file, "Process not open!");
		FILE* file = (FILE*)m_file;
		m_exitCode = pclose(file);
		m_file = NULL;
	}

	int32_t ProcessWriter::write(const void* _data, int32_t _size, Error* _err)
	{
		BX_CHECK(NULL != _err, "Reader/Writer interface calling functions must handle errors."); BX_UNUSED(_err);

		FILE* file = (FILE*)m_file;
		int32_t size = (int32_t)fwrite(_data, 1, _size, file);
		if (size != _size)
		{
			if (0 != ferror(file) )
			{
				BX_ERROR_SET(_err, BX_ERROR_READERWRITER_WRITE, "ProcessWriter: write error.");
			}

			return size >= 0 ? size : 0;
		}

		return size;
	}

	int32_t ProcessWriter::getExitCode() const
	{
		return m_exitCode;
	}
#endif // BX_CONFIG_CRT_PROCESS

} // namespace bx
