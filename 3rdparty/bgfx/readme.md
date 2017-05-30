bgfx.cmake
===================
[![Build Status](https://travis-ci.org/JoshuaBrookover/bgfx.cmake.svg?branch=master)](https://travis-ci.org/JoshuaBrookover/bgfx.cmake)

This repo contains a bunch of cmake files that can be used to build bgfx with CMake.

Building
-------------

```
git clone https://github.com/JoshuaBrookover/bgfx.cmake.git
cd bgfx.cmake
git submodule init
git submodule update
mkdir build
cd build
cmake ..
```

If downloading via zip (instead of using git submodules) manually download bx, bimg and bgfx and copy them into the root directory, or locate them via BX_DIR, BIMG_DIR and BGFX_DIR CMake variables.

Note
-------------
I try to maintain this repo when I can, but I have a full time job which makes it a bit difficult. I will gladly accept any pull requests updating to the latest bgfx, fixing bugs, improving support, etc.

How To Use
-------------
This project is setup to be included a few different ways. To include bgfx source code in your project simply use add_subdirectory to include this project. To build bgfx binaries build the INSTALL target (or "make install"). The installed files will be in the directory specified by CMAKE_INSTALL_PREFIX which I recommend you set to "./install" so it will export to your build directory. Note you may want to build install on both Release and Debug configurations.

Features
-------------
* No outside dependencies besides bx, bimg, bgfx, and CMake.
* Tested on Visual Studio 2015, Xcode, gcc 5.4, clang 3.8.
* Compiles bgfx, tools & examples.
* Detects shader modifications and automatically rebuilds them for all examples.

Todo
-------------
* Support Android.
* Support Native Client.
* Support Windows Phone.
* Build texturec and texturev.
* More configuration.
* Add varying.def.sc files as shader dependencies.
