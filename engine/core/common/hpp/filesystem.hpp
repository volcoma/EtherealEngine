#pragma once
#include <array>
#include <cstdlib>

#if defined(__cplusplus) && __cplusplus >= 201703L
#if defined(__has_include) && __has_include(<filesystem>)
#include <filesystem>
namespace fs
{
using namespace std::filesystem;
} // namespace fs
#endif
#else
#include "detail/filesystem_impl.hpp"

namespace fs
{
using namespace ghc::filesystem;
} // namespace fs
#endif

namespace fs
{

using error_code = std::error_code;
inline file_time_type now()
{
    return file_time_type::clock::now();
}
//-----------------------------------------------------------------------------
//  Name : executable_path()
/// <summary>
/// Retrieve the directory of the currently running application.
/// </summary>
//-----------------------------------------------------------------------------
path executable_path(const char* argv0);

//-----------------------------------------------------------------------------
//  Name : show_in_graphical_env ()
/// <summary>
/// Shows a path in the graphical environment e.g Explorer, Finder... etc.
/// </summary>
//-----------------------------------------------------------------------------
void show_in_graphical_env(const path& _path);

//-----------------------------------------------------------------------------
//  Name : persistent_path ()
/// <summary>
/// Returns os specific persistent folder like AppData on windows etc.
/// </summary>
//-----------------------------------------------------------------------------
path persistent_path();
}


namespace fs
{
inline path executable_path_fallback(const char* argv0)
{
    if(nullptr == argv0 || 0 == argv0[0])
    {
        return "";
    }
    fs::error_code err;
    path full_path(absolute(path(std::string(argv0)), err));
    return full_path;
}
}
#if defined(_WIN32)
#include <Windows.h>
#include <shlobj.h>

#undef min
#undef max
namespace fs
{
inline path executable_path(const char* argv0)
{
    std::array<char, 1024> buf;
    buf.fill(0);
    DWORD ret = GetModuleFileNameA(nullptr, buf.data(), DWORD(buf.size()));
    if(ret == 0 || std::size_t(ret) == buf.size())
    {
        return executable_path_fallback(argv0);
    }
    return path(std::string(buf.data()));
}
inline void show_in_graphical_env(const path& _path)
{
    ShellExecuteA(nullptr, nullptr, _path.string().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

inline path persistent_path()
{
    TCHAR szPath[MAX_PATH];

    if(SUCCEEDED(SHGetFolderPath(NULL,
                                 CSIDL_APPDATA,
                                 NULL,
                                 0,
                                 szPath)))
    {
        return path(szPath);
    }
    return {};
}
}
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
namespace fs
{
inline path executable_path(const char* argv0)
{
    std::array<char, 1024> buf;
    buf.fill(0);
    uint32_t size = buf.size();
    int ret = _NSGetExecutablePath(buf.data(), &size);
    if(0 != ret)
    {
        return executable_path_fallback(argv0);
    }
    fs::error_code err;
    path full_path(absolute(fs::absolute(path(std::string(buf.data()))), err));
    return full_path;
}
inline void show_in_graphical_env(const path& _path)
{
}
inline path persistent_path()
{
    return {};
}
}
#elif defined (__linux__)

#include <unistd.h>
namespace fs
{
inline path executable_path(const char* argv0)
{
    std::array<char, 1024> buf;
    buf.fill(0);

    ssize_t size = readlink("/proc/self/exe", buf.data(), buf.size());
    if(size == 0 || size == sizeof(buf))
    {
        return executable_path_fallback(argv0);
    }
    std::string p(buf.data(), size);
    fs::error_code err;
    path full_path(absolute(fs::absolute(path(p)), err));
    return full_path;
}
inline void show_in_graphical_env(const path& _path)
{
    static std::string cmd = "xdg-open";
    static std::string space = " ";
    const std::string cmd_args = "'" + _path.string() + "'";
    const std::string whole_command = cmd + space + cmd_args;
    auto result = std::system(whole_command.c_str());
    (void)result;
}
inline path persistent_path()
{

    char* home = getenv("XDG_CONFIG_HOME");
    if (!home)
    {
        home = getenv("HOME");

        if(!home)
        {
            return {};
        }
    }

    path result(home);
    result /= ".local/share";
    return result;
}
}
#else
namespace fs
{
inline path executable_path(const char* argv0)
{
    return executable_path_fallback(argv0);
}

inline void show_in_graphical_env(const path& _path)
{
}

inline path persistent_path()
{
    return {};
}
}
#endif
