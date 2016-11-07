#include "FileSystem.h"
#include "Core/common/string_utils.h"
#include "Core/platform_config.h"

namespace fs
{

	static std::string	mRootDirectory;     // The main file system root directory.
	static ProtocolMap	mProtocols;

	bool setRootDirectory(const std::string & strDir)
	{
		// Duplicate the data path (trim off any white space)
		mRootDirectory = string_utils::trim(strDir);

		// Skip if empty
		if (mRootDirectory.empty())
			return true;

		// No trailing slash?
		if (mRootDirectory.substr(mRootDirectory.length() - 1) != "\\" &&
			mRootDirectory.substr(mRootDirectory.length() - 1) != "/")
		{
			// Append a slash to the end of the data path
			mRootDirectory += "\\";

		} // End if no trailing slash

		// Success!
		return true;
	}

	bool addPathProtocol(const std::string & strProtocol, const std::string & strDir)
	{
		// Duplicate the data directory (trim off any white space)
		std::string strNewDir = string_utils::trim(strDir);
		std::string strNewProtocol = string_utils::trim(strProtocol);
		// Protocol matching is case insensitive, convert to lower case
		strNewProtocol = string_utils::toLower(strNewProtocol);

		// Append a slash to the end of the data path if required
		if (!strNewDir.empty() &&
			!string_utils::endsWith(strNewDir, "\\") &&
			!string_utils::endsWith(strNewDir, "/"))
			strNewDir.append("\\");

		// Add to the list
		mProtocols[strNewProtocol] = strNewDir;

		// Success!
		return true;
	}

	ProtocolMap& getProtocols()
	{
		return mProtocols;
	}

	const std::string& getRootDirectory()
	{
		return mRootDirectory;
	}

	bool pathProtocolDefined(const std::string & strProtocol)
	{
		// Protocol matching is case insensitive, convert to lower case
		std::string strKey = strProtocol;
		string_utils::toLower(strKey);
		return (mProtocols.find(strKey) != mProtocols.end());
	}

	void shutdown()
	{
	}

	ByteArray readStream(std::istream & stream)
	{
		// Open the stream
		ByteArray read_memory;
		if (stream) {
			// get length of file:
			stream.seekg(0, stream.end);
			size_t length = static_cast<std::size_t>(stream.tellg());
			stream.seekg(0, stream.beg);

			read_memory.resize(length, '\0'); // reserve space
			char* begin = (char*)&*read_memory.begin();

			stream.read(begin, length);

			stream.clear();
			stream.seekg(0);
		}
		// Done
		return read_memory;
	}

	std::string getDirectoryName(const std::string & strPathFile)
	{
		std::string::size_type nLastSlash, nLastSlash2;

		// Return the path portion only
		nLastSlash = strPathFile.rfind('\\');
		nLastSlash2 = strPathFile.rfind('/');
		if (nLastSlash == std::string::npos || (nLastSlash2 != std::string::npos && nLastSlash2 > nLastSlash))
			nLastSlash = nLastSlash2;

		// Any slash found?
		if (nLastSlash != std::string::npos)
			return strPathFile.substr(0, nLastSlash);

		// No path
		return std::string();
	}

	std::string getFileName(const std::string & strPathFile)
	{
		std::string              strFileName = strPathFile;
		std::string::size_type   nLastSlash;

		// Get filename only portion of the specified path
		nLastSlash = strPathFile.rfind('\\');
		if (nLastSlash == std::string::npos) nLastSlash = strPathFile.rfind('/');
		if (nLastSlash != std::string::npos) strFileName = strPathFile.substr(nLastSlash + 1);

		// Return result
		return strFileName;
	}

	std::string getFileName(const std::string & strPathFile, bool bStripExtension)
	{
		std::string              strFileName = strPathFile;
		std::string::size_type   nLastSlash;

		// Get filename only portion of the specified path
		nLastSlash = strPathFile.rfind('\\');
		if (nLastSlash == std::string::npos) nLastSlash = strPathFile.rfind('/');
		if (nLastSlash != std::string::npos) strFileName = strPathFile.substr(nLastSlash + 1);

		// Strip extension?
		if (bStripExtension)
		{
			// Find the portion of the specified file
			std::string::size_type nLastDot = strFileName.rfind('.');
			if (nLastDot != std::string::npos)
				strFileName = strFileName.substr(0, nLastDot);

		} // End if strip extension

		// Return result
		return strFileName;
	}

	std::string getFileNameExtension(const std::string & strPathFile)
	{
		std::string            strFileName;
		std::string::size_type nLastDot;

		// First ensure we only have the file name portion of a full path+file
		// std::string so that we don't mistake a 'dot' in the path as the extension
		// delimeter.
		strFileName = getFileName(strPathFile);

		// Get extension only portion of the specified file
		nLastDot = strFileName.rfind('.');
		if (nLastDot == std::string::npos) return std::string();

		// Return only the extension
		return strFileName.substr(nLastDot + 1);
	}

	std::string ensurePath(const std::string & strPathFile, bool resolve)
	{
		std::string filepath = strPathFile;
		if (resolve)
			filepath = resolveFileLocation(strPathFile);

		std::string directoryName = getDirectoryName(filepath);

		if (!directoryExists(directoryName))
		{
			createDirectory(directoryName);
		}

		return filepath;
	}

}