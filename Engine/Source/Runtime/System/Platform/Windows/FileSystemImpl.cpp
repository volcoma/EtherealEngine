#include "System/FileSystem.h"

#include "Core/common/string_utils.h"
//Windows platform includes
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlobj.h>
#undef WIN32_LEAN_AND_MEAN

// Runtime includes
#include <io.h>                 // For access().
#include <sys/types.h>          // For stat().
#include <sys/stat.h>           // For stat().
#include <algorithm>
#include <tchar.h>
#include <direct.h>
// Remove windows #DEFINE interference
#undef createDirectory

namespace fs
{

//-----------------------------------------------------------------------------
//  Name : getAppDirectory() (Static)
/// <summary>
/// Retrieve the directory of the currently running application.
/// </summary>
//-----------------------------------------------------------------------------
std::string getAppDirectory()
{
	char strBuffer[MAX_PATH];

    // Retrieve the module name for the process we're running in
    ::GetModuleFileNameA( nullptr, strBuffer, MAX_PATH - 1 );
    
    // Return the direction portion only
    return getDirectoryName( strBuffer ) + _T("\\");
}

//-----------------------------------------------------------------------------
//  Name : getModuleName() (Static)
/// <summary>
/// Retrieves the fully qualified path for the file that contains the specified module.
/// </summary>
//-----------------------------------------------------------------------------
std::string getModuleName()
{
	char strBuffer[MAX_PATH];

	// Retrieve the module name for the process we're running in
	::GetModuleFileNameA(nullptr, strBuffer, MAX_PATH - 1);

	// Return the direction portion only
	return strBuffer;
}


//-----------------------------------------------------------------------------
//  Name : fileExists () (Static)
/// <summary>
/// Given a full path name, determine if it exists (and is a file).
/// </summary>
//-----------------------------------------------------------------------------
bool fileExists(const std::string & strPathFile)
{
	// First, let's see if the referenced item actually exists on disk
	if ( _access( strPathFile.c_str(), 0 ) == 0 )
	{
	    // Item exists. Is it a file or directory?
	    struct stat status;
		stat(strPathFile.c_str(), &status);
	
	    // Must not indicate directory.
	    if ( status.st_mode & S_IFDIR )
	        return false;
	
	    // Item is a file
	    return true;
	
	} // End if exists
	
	// File does not exist
	return false;
}

//-----------------------------------------------------------------------------
//  Name : directoryExists () (Static)
/// <summary>
/// Given a full directory name, determine if it exists (and is a directory).
/// </summary>
//-----------------------------------------------------------------------------
bool directoryExists(const std::string & strDirectory)
{
	// First, let's see if the referenced item actually exists on disk
	if ( _access( strDirectory.c_str(), 0 ) == 0 )
	{
	    // Item exists. Is it a file or directory?
	    struct stat status;
		stat(strDirectory.c_str(), &status);
	
	    // Must indicate directory.
	    if ( status.st_mode & S_IFDIR )
	        return true;
	
	    // Item is a file
	    return false;
	
	} // End if exists
	
	// Directory does not exist
	return false;
}

//-----------------------------------------------------------------------------
//  Name : getAbsolutePath () (Static)
/// <summary>
/// This function will return a path (and file) which is the absolute
/// version of the path specified relative to the current directory.
/// Note : This function is mildly expensive and should be used sparingly.
/// </summary>
//-----------------------------------------------------------------------------
std::string getAbsolutePath(const std::string & strPathFile, const std::string & strCurrentDir /* = "" */)
{
	std::string CurrentPath, RelativePath;
	char OldCD[MAX_PATH], AbsolutePath[MAX_PATH];
	memset(OldCD, 0, MAX_PATH * sizeof(char));
	memset(AbsolutePath, 0, MAX_PATH * sizeof(char));
	// Trim off any white space
	CurrentPath = string_utils::trim(strCurrentDir);
	RelativePath = string_utils::trim(strPathFile);
	
	   // Replace all forward slashes, with back slashes
	CurrentPath = string_utils::replace(CurrentPath, '/', '\\');
	RelativePath = string_utils::replace(RelativePath, '/', '\\');
	   
	   // Strip out ANY double back slashes as these are invalid
	CurrentPath = string_utils::replace(CurrentPath, "\\\\", "\\");
	RelativePath = string_utils::replace(RelativePath, "\\\\", "\\");
	
	// Change the current directory (if overriding)
	if (!strCurrentDir.empty())
	{
		// Backup old current dir
		if (!::_getcwd(OldCD, MAX_PATH))
			return std::string();

		// Set the current directory for expansion
		if (::_chdir(CurrentPath.c_str()) == -1)
			return std::string();

	} // End if overriding dir

	// Expand the relative path
	_fullpath(AbsolutePath, RelativePath.c_str(), MAX_PATH);

	// Restore the previous current directory (if overriden)
	if (!strCurrentDir.empty())
		_chdir(OldCD);

	// Return the absolute path
	return std::string(AbsolutePath);
}

//-----------------------------------------------------------------------------
//  Name : getRelativePath () (Static)
/// <summary>
/// This function will return a path (and file) relative to the path 
/// specified in the CurrentDir parameter.
/// Note : This function is mildly expensive and should be used sparingly.
/// </summary>
//-----------------------------------------------------------------------------
std::string getRelativePath(const std::string & strPathFile, const std::string & strCurrentDir /* = "" */)
{
	std::string CurrentPath = strCurrentDir;
	std::string AbsolutePath = strPathFile;
	std::string RelativePath;
	std::string::size_type i;

	// If the specified current directory is empty, use the actual current dir
	if (strCurrentDir.empty())
	{
		char Buffer[MAX_PATH];
		if (!::_getcwd(Buffer, MAX_PATH))
			return std::string();
		CurrentPath = Buffer;

	} // End if no override dir

	// Ensure both are the same case
	CurrentPath = string_utils::trim(CurrentPath);
	AbsolutePath = string_utils::trim(AbsolutePath);
	CurrentPath = string_utils::toLower(CurrentPath);
	AbsolutePath = string_utils::toLower(AbsolutePath);
	// Has any length at all after the trim etc
	if (AbsolutePath.length() == 0)
		return strPathFile;

	// Replace all forward slashes, with back slashes
	CurrentPath = string_utils::replace(CurrentPath, '/', '\\');
	AbsolutePath = string_utils::replace(AbsolutePath, '/', '\\');

	// Strip out ANY double back slashes as these are invalid
	CurrentPath = string_utils::replace(CurrentPath, "\\\\", "\\");
	AbsolutePath = string_utils::replace(AbsolutePath, "\\\\", "\\");
	// Handle names that are on different drives
	if (CurrentPath[0] != AbsolutePath[0])
	{
		// These are not on the same drive so only absolute will do
		return strPathFile;

	} // End If Not Same Drive

	// Determine how much of both buffers are identical
	for (i = 0; i < CurrentPath.length() && i < AbsolutePath.length(); i++)
	{
		if (CurrentPath[i] != AbsolutePath[i])
			break;

	} // Next Character

	// Did we get to the end of the source??
	if (i == CurrentPath.length() && (AbsolutePath[i] == '\\' || AbsolutePath[i - 1] == '\\'))
	{
		// The whole source directory name is in the target name so we just trim and return
		// Trim off any leading slash first however.
		if (AbsolutePath[i] == '\\')
			i++;
		return strPathFile.substr(i);

	} // End If

	// The file is not in a child directory of the current so we
	// need to step back the appropriate number of parent directories
	// First we will find out how many leves deeper we are than the source.
	std::string::size_type afMarker = i;
	std::string::size_type levels = 1;
	while (i < CurrentPath.length())
	{
		// We've found a slash
		if (CurrentPath[i] == '\\')
		{
			// Ensure it's not a trailing slash
			i++;
			if (i != CurrentPath.length())
				levels++;

		} // End If
		i++;
	} // Next Character

	// Move the absolute filename marker back to the start of the dir name that it stopped in
	while (afMarker > 0 && AbsolutePath[afMarker - 1] != '\\')
		afMarker--;

	// Add the appropriate number of "..\"s.
	for (i = 0; i < levels; i++)
		RelativePath += "..\\";

	// Copy the rest of the filename into the result std::string
	RelativePath += strPathFile.substr(afMarker);

	// Return our relative path
	return RelativePath;
}
//-----------------------------------------------------------------------------
//  Name : createDirectory () (Static)
/// <summary>
/// Create the specified directory, including any full parent sub-tree as
/// required.
/// </summary>
//-----------------------------------------------------------------------------
bool createDirectory( const std::string & strDirectory )
{
    return ( SHCreateDirectoryExA( NULL, strDirectory.c_str(), NULL ) == ERROR_SUCCESS );
}

//-----------------------------------------------------------------------------
//  Name : getTemporaryFile () (Static)
/// <summary>
/// Generate a new (empty) temporary file located in the system's configured 
/// temporary file directory and return the full path to that file. Returns an 
/// empty string if a suitable temporary file name or associated empty file 
/// failed to be generated.
/// </summary>
//-----------------------------------------------------------------------------
std::string getTemporaryFile()
{
    //  Retrieve the temporary directory environment variable.
    char tempPathBuffer[MAX_PATH];
    std::uint32_t result = GetTempPathA(MAX_PATH, tempPathBuffer );
    if ( result > MAX_PATH || (result == 0))
        return "";
    
    //  Generates a temporary file / file name. 
    char tempFileName[MAX_PATH];  
    if ( !GetTempFileNameA( tempPathBuffer, _T("TEMP_"), 0, tempFileName ) )
        return "";

    // We're done.
    return tempFileName;
}

//-----------------------------------------------------------------------------
//  Name : getTemporaryFile () (Static)
/// <summary>
/// Generate a new (empty) temporary file located in the specified directory
/// and return the full path to that file. Returns an empty string if a 
/// suitable temporary file name or associated empty file failed to be 
/// generated.
/// </summary>
//-----------------------------------------------------------------------------
std::string getTemporaryFile( const std::string & basePath )
{
    // Generates a temporary file / file name. 
    char tempFileName[MAX_PATH];  
    if ( !GetTempFileNameA( basePath.c_str(), _T("TEMP_"), 0, tempFileName ) )
        return "";

    // We're done.
    return tempFileName;
}

//-----------------------------------------------------------------------------
//  Name : copyFile () (Static)
/// <summary>
/// Copy the specified source file to a new location. If the destination file
/// already exists and the caller supplies a value of 'false' to the 
/// 'overwriteExisting', the call will fail. Otherwise, the destination file
/// will be overwritten (where possible). Returns false if the copy operation 
/// failed for this or any other reason.
/// </summary>
//-----------------------------------------------------------------------------
bool copyFile( const std::string & source, const std::string & destination, bool overwriteExisting )
{
    return (CopyFileA( source.c_str(), destination.c_str(), !overwriteExisting) != 0);
}

//-----------------------------------------------------------------------------
//  Name : moveFile () (Static)
/// <summary>
/// Move the specified source file to a new location. This function can also
/// be used to rename a file. Returns false if the move / rename operation 
/// failed for any reason.
/// </summary>
//-----------------------------------------------------------------------------
bool moveFile( const std::string & source, const std::string & destination, bool overwriteExisting )
{
    std::uint32_t flags = MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH;
    if ( overwriteExisting )
        flags |= MOVEFILE_REPLACE_EXISTING;
    return (MoveFileExA( source.c_str(), destination.c_str(), flags ) != 0);
}

//-----------------------------------------------------------------------------
//  Name : deleteFile () (Static)
/// <summary>
/// Delete the specified file. Returns false if the delete operation failed
/// for any reason, i.e. the file was not found, or access was denied.
/// </summary>
//-----------------------------------------------------------------------------
bool deleteFile( const std::string & pathFile )
{
    return (DeleteFileA( pathFile.c_str() ) != 0);
}

//-----------------------------------------------------------------------------
//  Name : deleteFile () (Static)
/// <summary>
/// Delete the specified file. Returns false if the delete operation failed
/// for any reason, i.e. the file was not found, or access was denied.
/// </summary>
//-----------------------------------------------------------------------------
bool createFile( const std::string & pathFile )
{
	HANDLE hFile = CreateFileA( pathFile.c_str() ,GENERIC_READ,	0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

	bool created = hFile != 0;

	if(created)
		CloseHandle(hFile);

	return created;
}

//-----------------------------------------------------------------------------
//  Name : isSameFile () (Static)
/// <summary>
/// Attempt to determine if the two specified paths / files reference the same 
/// file on disk. User must have sufficient permissions to access both files
/// in order for this function to succeed. This version of the function 
/// returns extended success or failure information via its third parameter.
/// Note: May return 'false' when paths reference the same file via two 
/// separate network shares.
/// </summary>
//-----------------------------------------------------------------------------
bool isSameFile( const std::string & file1, const std::string & file2, bool & success )
{
    // Failed by default
    success = true;

    // Get handles to both files.
    HANDLE h1 = CreateFileA( file1.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 
                            nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_ATTRIBUTE_NORMAL, nullptr );
    if ( !h1 )
        return false;
    
    HANDLE h2 = CreateFileA( file2.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 
                            nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_ATTRIBUTE_NORMAL, nullptr );
    if ( !h2 )
    {
        if ( h1 )
            CloseHandle(h1);
        return false;
    
    } // End if create failed

    // Get file information for both files
    bool result = false;
    BY_HANDLE_FILE_INFORMATION info1, info2;
    if ( GetFileInformationByHandle( h1, &info1 ) && GetFileInformationByHandle( h2, &info2 ) )
    {
        success = true;

        // Reference the same file?
        if ( info1.dwVolumeSerialNumber == info2.dwVolumeSerialNumber &&
             info1.nFileIndexHigh == info2.nFileIndexHigh &&
             info1.nFileIndexLow == info2.nFileIndexLow )
             result = true;

    } // End if query success

    // Clean up
    CloseHandle(h1);
    CloseHandle(h2);

    // We're done.
    return result;
}


//-----------------------------------------------------------------------------
//  Name : resolveFileLocation() (Static)
/// <summary>
/// Given the specified path/filename, resolve the final full filename.
/// This will be based on either the currently specified root path,
/// or one of the 'path protocol' identifiers if it is included in the
/// filename.
/// </summary>
//-----------------------------------------------------------------------------
std::string resolveFileLocation(const std::string & strFile)
{
	// Case insensitive test please
	std::string strLocation = string_utils::toLower(strFile);
	// Includes a path protocol at the start?
	std::string::size_type nSeparator = strLocation.find("://");
	if (nSeparator != std::string::npos)
	{
		// Extract the protocol
		std::string strProtocol = strLocation.substr(0, nSeparator);
		strLocation = strFile.substr(nSeparator + 3);

		// Matching path protocol in our list?
		auto& protocols = getProtocols();
		auto itProtocol = protocols.find(strProtocol);
		if (itProtocol == std::end(protocols))
			return strFile;

		// Found matching path protocol. Canonicalize.
		char result[MAX_PATH];
		GetFullPathNameA((itProtocol->second + strLocation).c_str(), MAX_PATH, result, nullptr);
		return result;

	} // End if includes protocol
	else if (strLocation.length() > 2 && strLocation[1] == _T(':'))
	{
		// This is an absolute drive path (i.e. c:\Test.jpg).
		char result[MAX_PATH];
		GetFullPathNameA(strFile.c_str(), MAX_PATH, result, nullptr);
		return result;

	} // End if absolute path

	// No protocol, just return with root path pre-pended
	char result[MAX_PATH];
	GetFullPathNameA((getRootDirectory() + strFile).c_str(), MAX_PATH, result, nullptr);
	return result;
}

}