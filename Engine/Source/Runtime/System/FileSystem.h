#pragma once

#include <unordered_map>
#include <istream>
#include <filesystem>

namespace fs
{
	using namespace std::experimental::filesystem;
	using ProtocolMap = std::unordered_map<std::string, std::string>;
	using ByteArray = std::vector<std::uint8_t>;

	//-----------------------------------------------------------------------------
	//  Name : shutdown ()
	/// <summary>
	/// Shut down the file system and clean up allocated data.
	/// </summary>
	//-----------------------------------------------------------------------------
	void shutdown();

	//-----------------------------------------------------------------------------
	//  Name : setRootDirectory ()
	/// <summary>
	/// Set the directory that the file system will consider to be its root.
	/// When indexing, this will be the base path in which we recurse.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool setRootDirectory(const std::string& directoryName);

	//-----------------------------------------------------------------------------
	//  Name : getRootDirectory ()
	/// <summary>
	/// Gets the directory that the file system will consider to be its root.
	/// </summary>
	//-----------------------------------------------------------------------------
	const std::string& getRootDirectory();

	//-----------------------------------------------------------------------------
	//  Name : addPathProtocol ()
	/// <summary>
	/// Allows us to map a protocol to a specific directory. A path protocol
	/// gives the caller the ability to prepend an identifier to their file
	/// name i.e. "sys://Textures/MyTex.png" and have it return the
	/// relevant mapped path.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool addPathProtocol(const std::string& protocol, const std::string& directoryName);

	//-----------------------------------------------------------------------------
	//  Name : getProtocols ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	ProtocolMap& getProtocols();

	//-----------------------------------------------------------------------------
	//  Name : pathProtocolDefined ()
	/// <summary>
	/// Determine if a path protocol with the specified name has been defined.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool pathProtocolDefined(const std::string& protocol);

	//-----------------------------------------------------------------------------
	//  Name : readStream ()
	/// <summary>
	/// Load a std::string with the contents of the specified file, be that file in a
	/// package or in the main file system.
	/// </summary>
	//-----------------------------------------------------------------------------
	ByteArray readStream(std::istream& stream);

	//-----------------------------------------------------------------------------
	//  Name : getFileNameExtension ()
	/// <summary>
	/// Given a full path name, return just the extension portion of it.
	/// </summary>
	//-----------------------------------------------------------------------------
	std::string getFileNameExtension(const std::string& pathFile);

	//-----------------------------------------------------------------------------
	//  Name : getFileName ()
	/// <summary>
	/// Given a full path name, return just the filename portion of it.
	/// </summary>
	//-----------------------------------------------------------------------------
	std::string getFileName(const std::string& pathFile);

	//-----------------------------------------------------------------------------
	//  Name : getFileName ()
	/// <summary>
	/// Given a full path name, return just the filename portion of it. Optionally
	/// the extension can be automatically stripped.
	/// </summary>
	//-----------------------------------------------------------------------------
	std::string getFileName(const std::string& pathFile, bool stripExtension);

	//-----------------------------------------------------------------------------
	//  Name : getDirectoryName ()
	/// <summary>
	/// Given a full path name, return just the direction portion of it.
	/// </summary>
	//-----------------------------------------------------------------------------
	std::string getDirectoryName(const std::string& pathFile);

	//-----------------------------------------------------------------------------
	//  Name : ensurePath ()
	/// <summary>
	/// This function will create a directory if it doesn't exist.
	/// 'pathFile' can be either relative e.g "sys://assets/file.txt
	/// but then the resolve should be set to true or it can be an absolute path.
	/// </summary>
	//-----------------------------------------------------------------------------
	std::string ensurePath(const std::string& pathFile, bool resolove);

	//-----------------------------------------------------------------------------
	//  Name : getAbsolutePath ()
	/// <summary>
	/// This function will return a path (and file) which is the absolute
	/// version of the path specified relative to the current directory.
	/// Note : This function is mildly expensive and should be used sparingly.
	/// </summary>
	//-----------------------------------------------------------------------------
	std::string getAbsolutePath(const std::string& pathFile, const std::string& currentDirectory = "");

	//-----------------------------------------------------------------------------
	//  Name : getRelativePath ()
	/// <summary>
	/// This function will return a path (and file) relative to the path 
	/// specified in the CurrentDir parameter.
	/// Note : This function is mildly expensive and should be used sparingly.
	/// </summary>
	//-----------------------------------------------------------------------------
	std::string getRelativePath(const std::string& pathFile, const std::string& currentDirectory = "");

	//-----------------------------------------------------------------------------
	//  Name : fileExists ()
	/// <summary>
	/// Given a full path name, determine if it exists (and is a file).
	/// </summary>
	//-----------------------------------------------------------------------------
	bool fileExists(const std::string& pathFile);

	//-----------------------------------------------------------------------------
	//  Name : directoryExists ()
	/// <summary>
	/// Given a full directory name, determine if it exists (and is a directory).
	/// </summary>
	//-----------------------------------------------------------------------------
	bool directoryExists(const std::string& directoryName);

	//-----------------------------------------------------------------------------
	//  Name : resolveFileLocation()
	/// <summary>
	/// Given the specified path/filename, resolve the final full filename.
	/// This will be based on either the currently specified root path,
	/// or one of the 'path protocol' identifiers if it is included in the
	/// filename.
	/// </summary>
	//-----------------------------------------------------------------------------
	std::string resolveFileLocation(const std::string& fileName);

	//-----------------------------------------------------------------------------
	//  Name : getAppDirectory()
	/// <summary>
	/// Retrieve the directory of the currently running application.
	/// </summary>
	//-----------------------------------------------------------------------------
	std::string getAppDirectory();

	//-----------------------------------------------------------------------------
	//  Name : getModuleName() (Static)
	/// <summary>
	/// Retrieves the fully qualified path for the file that contains the specified module.
	/// </summary>
	//-----------------------------------------------------------------------------
	std::string getModuleName();

	//-----------------------------------------------------------------------------
	//  Name : getTemporaryFile ()
	/// <summary>
	/// Generate a new (empty) temporary file located in the system's configured 
	/// temporary file directory and return the full path to that file. Returns an 
	/// empty string if a suitable temporary file name or associated empty file 
	/// failed to be generated.
	/// </summary>
	//-----------------------------------------------------------------------------
	std::string getTemporaryFile();

	//-----------------------------------------------------------------------------
	//  Name : getTemporaryFile ()
	/// <summary>
	/// Generate a new (empty) temporary file located in the specified directory
	/// and return the full path to that file. Returns an empty string if a 
	/// suitable temporary file name or associated empty file failed to be 
	/// generated.
	/// </summary>
	//-----------------------------------------------------------------------------
	std::string getTemporaryFile(const std::string& basePath);

	//-----------------------------------------------------------------------------
	//  Name : createDirectory ()
	/// <summary>
	/// Create the specified directory, including any full parent sub-tree as
	/// required.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool createDirectory(const std::string& directoryName);

	//-----------------------------------------------------------------------------
	//  Name : copyFile ()
	/// <summary>
	/// Copy the specified source file to a new location. If the destination file
	/// already exists and the caller supplies a value of 'false' to the 
	/// 'overwriteExisting', the call will fail. Otherwise, the destination file
	/// will be overwritten (where possible). Returns false if the copy operation 
	/// failed for this or any other reason.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool copyFile(const std::string& source, const std::string& destination, bool overwriteExisting);

	//-----------------------------------------------------------------------------
	//  Name : moveFile ()
	/// <summary>
	/// Move the specified source file to a new location. This function can also
	/// be used to rename a file. Returns false if the move / rename operation 
	/// failed for any reason.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool moveFile(const std::string& source, const std::string& destination, bool overwriteExisting);

	//-----------------------------------------------------------------------------
	//  Name : deleteFile ()
	/// <summary>
	/// Delete the specified file. Returns false if the delete operation failed
	/// for any reason, i.e. the file was not found, or access was denied.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool deleteFile(const std::string& pathFile);

	//-----------------------------------------------------------------------------
	//  Name : deleteFile ()
	/// <summary>
	/// Delete the specified file. Returns false if the delete operation failed
	/// for any reason, i.e. the file was not found, or access was denied.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool createFile(const std::string& pathFile);

	//-----------------------------------------------------------------------------
	//  Name : isSameFile ()
	/// <summary>
	/// Attempt to determine if the two specified paths / files reference the same 
	/// file on disk. User must have sufficient permissions to access both files
	/// in order for this function to succeed. This version of the function 
	/// returns extended success or failure information via its third parameter.
	/// Note: May return 'false' when paths reference the same file via two 
	/// separate network shares.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool isSameFile(const std::string& file1, const std::string& file2, bool& success);
};
