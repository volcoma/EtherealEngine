## INFO
Cross-platform C++ Game Engine in c++14

WYSIWYG Editor

## STATUS
WIP - not production ready in any way

## FEATURES
Check out the Projects tab for more information of upcomming features.

## ASSETS
Supported texture formats: png, tga, dds, ktx, pvr

Supported mesh formats: obj, fbx, dae, 3ds, blend

## SUGGESTIONS AND ISSUES
Any suggestions and help will be appreciated.

## BUILD
*The engine uses the CMake build system.


## PLATFORMS
It is written in a cross-platform manner using c++14.

Compilers      | Windows | Linux
---------------| --------|--------
MSVC(2015 +)   | yes     |
MinGW          | yes     |
GCC(5.4 +)     |         | yes
clang(3.8 +)   |         | yes

## CODEBASE
c++14 Using the latest and greatest features of the language.

## CODE STYLE
```c++
#include "some_header.h"
#include "some_other_header_impl.hpp"

namespace nsp
{
class some_class
{
public:
  //-----------------------------------------------------------------------------
  //  Name : some_method ()
  /// <summary>
  /// This is a method comment description.
  /// </summary>
  //-----------------------------------------------------------------------------
  void some_method();
  
  //-----------------------------------------------------------------------------
  //  Name : some_templated_method ()
  /// <summary>
  /// This is a method comment description.
  /// </summary>
  //-----------------------------------------------------------------------------
  template<typename T>
  void some_templated_method();
public:
  /// this is a member comment
  int some_public_member = 0;
  
private:
  /// this is a member comment
  int _some_private_member = 0;
}

template<typename T>
inline void some_class::some_templated_method()
{
// If the class is fully templated it is acceptable to put this inside the class
}
```

## LICENSE
[LINK](LICENSE.md)

## PREVIEW
![prev](https://cloud.githubusercontent.com/assets/1499411/23671477/0a2016fa-0374-11e7-8456-0521c78c5f12.png)

![proceduralsky](https://cloud.githubusercontent.com/assets/1499411/22755670/d6cf794e-ee4c-11e6-92cd-e29ae30eefc8.png)

![proceduralsky1](https://cloud.githubusercontent.com/assets/1499411/22755669/d6cd60fa-ee4c-11e6-9ff2-ebf0e3535e39.png)


## LIBRARIES
bgfx - https://github.com/bkaradzic/bgfx

cereal - https://github.com/USCiLab/cereal

rttr - https://github.com/rttrorg/rttr

spdlog - https://github.com/gabime/spdlog

tracey - https://github.com/r-lyeh/tracey

imgui - https://github.com/ocornut/imgui
