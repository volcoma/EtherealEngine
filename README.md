[Ethereal Engine](https://github.com/volcoma/EtherealEngine) - Cross-platform C++ Game Engine
============================================================================

[![Join the chat at https://gitter.im/EtherealEngine/Lobby](https://badges.gitter.im/EtherealEngine/Lobby.svg)](https://gitter.im/EtherealEngine/Lobby?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
[![Build Status](https://travis-ci.org/volcoma/EtherealEngine.svg?branch=master)](https://travis-ci.org/volcoma/EtherealEngine)
[![Build status](https://ci.appveyor.com/api/projects/status/18pxyag4wok2fscl?svg=true)](https://ci.appveyor.com/project/volcoma/etherealengine)
[![](https://codescene.io/projects/1800/status.svg) Get more details at **codescene.io**.](https://codescene.io/projects/1800/jobs/latest-successful/results)

## INFO
Using c++14

WYSIWYG Editor

## STATUS
WIP - not production ready in any way

Building
-------------
Don't forget to update submodules
```
git clone https://github.com/volcoma/EtherealEngine.git
cd EtherealEngine
git submodule init
git submodule update

mkdir build
cd build
cmake ..

```

## PLATFORMS
It is written in a cross-platform manner using c++14.

Compilers      | Windows | Linux
---------------| --------|--------
MSVC(2015 +)   | yes     |
MinGW(5.3 +)   | yes     |
GCC(5.0 +)     |         | yes
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
