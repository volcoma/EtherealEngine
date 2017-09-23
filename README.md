## INFO
Cross-platform C++ Game Engine in c++14

WYSIWYG Editor

## STATUS
WIP - not production ready in any way

Building
-------------
Don't forget to update submodules
```
cd your_cloned_dir
git submodule init
git submodule update

```

## PLATFORMS
It is written in a cross-platform manner using c++14.

Compilers      | Windows | Linux
---------------| --------|--------
MSVC(2015 +)   | yes     |
MinGW          | yes     |
GCC(5.4 +)     |         | yes
clang(3.8 +)   |         | yes

## ASSETS
Some high quality assets: https://github.com/volcoma/Library

Supported texture formats: png, tga, dds, ktx, pvr

Supported mesh formats: obj, fbx, dae, 3ds

## SUGGESTIONS AND ISSUES
Any suggestions and help will be appreciated.

## BUILD
*The engine uses the CMake build system.

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
s  /// </summary>
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
[LINK](LICENSE.txt)

## PREVIEW
![screenshot1](https://user-images.githubusercontent.com/1499411/29488399-e4c76a18-8512-11e7-8090-7453fea8d33f.png)

![screenshot2](https://user-images.githubusercontent.com/1499411/29488400-f43a5960-8512-11e7-923e-86a0c204da31.png)

![screenshot3](https://user-images.githubusercontent.com/1499411/29488403-ff3c3df6-8512-11e7-869f-32a783530cc3.png)


## LIBRARIES
bgfx - https://github.com/bkaradzic/bgfx

cereal - https://github.com/USCiLab/cereal

rttr - https://github.com/rttrorg/rttr

spdlog - https://github.com/gabime/spdlog

imgui - https://github.com/ocornut/imgui

assimp - https://github.com/assimp/assimp

glm - https://github.com/g-truc/glm
