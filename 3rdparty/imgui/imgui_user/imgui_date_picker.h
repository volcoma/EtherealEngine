// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#ifndef IMGUIDATECHOOSER_H_
#define IMGUIDATECHOOSER_H_

#ifndef IMGUI_API
#include "../imgui/imgui.h"
#endif			 // IMGUI_API
#include <ctime> // very simple and common plain C header file (it's NOT the c++ <sys/time.h>). If not available it's probably better to implement it yourself rather than modifying this file.

// USAGE
/*
#include "imguidatechooser.h"
//#include <time.h>   // mandatory when implementing ImGui::TestDateChooser() yourself in your code

// inside a ImGui::Window:
ImGui::TestDateChooser();
*/

// struct tm;  // defined in <time.h>

namespace ImGui
{

// Some example of date formats: "%d/%m/%Y" "%A %d %b %Y"
IMGUI_API bool DateChooser(const char* label, tm& dateOut, const char* dateFormat = "%d/%m/%Y",
						   bool* pSetStartDateToDateOutThisFrame = NULL, const char* leftArrow = "<",
						   const char* rightArrow = ">", const char* upArrowString = "   ^   ",
						   const char* downArrowString = "   v   ");
// Some helper methods (note that I cannot use tm directly in this hader file, so we can't initialize a static
// date directly with these methods)
IMGUI_API void SetDateZero(tm* date);
IMGUI_API void SetDateToday(tm* date);

IMGUI_API void TestDateChooser(const char* dateFormat = "%d/%m/%Y", const char* leftArrow = "<",
							   const char* rightArrow = ">", const char* upArrowString = "   ^   ",
							   const char* downArrowString = "   v   ");

} // namespace ImGui

#endif
