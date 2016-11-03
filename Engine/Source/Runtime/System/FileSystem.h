#pragma once

#include <unordered_map>
#include <istream>
#include <filesystem>
//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : FileSystem (Static Class)
/// <summary>
/// Class that contains our core file system handling functionality.
/// Access is via entirely static methods to provide support application
/// wide.
/// </summary>
//-----------------------------------------------------------------------------
namespace fs
{
	using namespace std::experimental::filesystem;
	using ProtocolMap = std::unordered_map<std::string, std::string>;
	using ByteArray = std::vector<std::uint8_t>;
    //-------------------------------------------------------------------------
    // Public Static Functions
    //-------------------------------------------------------------------------
	void				shutdown            ( );
	bool				setRootDirectory    ( const std::string & directoryName );
	const std::string&	getRootDirectory	( );
	bool				addPathProtocol     ( const std::string & protocol, const std::string & directoryName );
	ProtocolMap&		getProtocols		( );
	bool				pathProtocolDefined ( const std::string & protocol );
	// Utility methods
	ByteArray			readStream			( std::istream & stream );
	std::string			getFileNameExtension( const std::string & pathFile );
	std::string			getFileName         ( const std::string & pathFile );
	std::string			getFileName         ( const std::string & pathFile, bool stripExtension );
	std::string			getDirectoryName    ( const std::string & pathFile );
	std::string			ensurePath			( const std::string & pathFile, bool resolove );

	std::string			getAbsolutePath     ( const std::string & pathFile, const std::string & currentDirectory = "" );
	std::string			getRelativePath     ( const std::string & pathFile, const std::string & currentDirectory = "" );
	bool				fileExists          ( const std::string & pathFile );
	bool				directoryExists     ( const std::string & directoryName );
    std::string			resolveFileLocation ( const std::string & fileName );
	std::string			getAppDirectory     ( );
	std::string			getModuleName       ( );
	std::string			getTemporaryFile    ( );
	std::string			getTemporaryFile    ( const std::string & basePath );
	bool				createDirectory     ( const std::string & directoryName );
	bool				copyFile            ( const std::string & source, const std::string & destination, bool overwriteExisting );
	bool				moveFile            ( const std::string & source, const std::string & destination, bool overwriteExisting );
	bool				deleteFile          ( const std::string & pathFile );
	bool				createFile          ( const std::string & pathFile );
	bool				isSameFile          ( const std::string & file1, const std::string & file2, bool & success );
};
