#pragma once

#include "filesystem_includes.h"

namespace fs
{
//-----------------------------------------------------------------------------
//  Name : executable_path()
/// <summary>
/// Retrieve the directory of the currently running application.
/// </summary>
//-----------------------------------------------------------------------------
auto executable_path(const char* argv0) -> path;

//-----------------------------------------------------------------------------
//  Name : show_in_graphical_env ()
/// <summary>
/// Shows a path in the graphical environment e.g Explorer, Finder... etc.
/// </summary>
//-----------------------------------------------------------------------------
void show_in_graphical_env(const path& _path);
} // namespace fs
