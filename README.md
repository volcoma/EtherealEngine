## INFO
C++ Game Engine

WYSIWYG Editor

## STATUS
WIP - not production ready in any way

## FEATURES
Proper rendering and lighting is pretty much non existant right now. PBR is planned.

Check out the Projects tab for more information of upcomming features.

## ASSETS
Supported texture formats: png, tga, dds, ktx, pvr

Supported mesh formats: obj only for now.

## SUGGESTIONS AND ISSUES
Any suggestions and help will be appreciated.

## BUILD
VS2015 solution is included in the repo. Just build it.

## PLATFORMS
It is written in a cross-platform manner.

Platform | Supported
--- | --- | ---
Windows | yes |
Linux | needs some work |

## CODEBASE
c++14/c++17 Using the latest and greates features of the language.

## CODE STYLE
```c++
class SomeClass
{
public:
  //-----------------------------------------------------------------------------
  //  Name : some_method ()
  /// <summary>
  /// This is a method comment description.
  /// </summary>
  //-----------------------------------------------------------------------------
  void some_method();
  
private:
  /// this is a member comment
  int _some_member = 0;
}
```

## PREVIEW
![preview](https://cloud.githubusercontent.com/assets/1499411/19988985/2a302204-a22c-11e6-98af-5f446d0c79ac.png)

![preview1](https://cloud.githubusercontent.com/assets/1499411/19989003/40535d44-a22c-11e6-9aa8-a1ddd63df18a.png)

![preview5](https://cloud.githubusercontent.com/assets/1499411/22198936/6a085efe-e161-11e6-8d76-3ba179e7c76b.png)

![preview6](https://cloud.githubusercontent.com/assets/1499411/22198950/78759b82-e161-11e6-9681-219d15f1482f.png)

## LIBRARIES
bgfx - https://github.com/bkaradzic/bgfx

cereal - https://github.com/USCiLab/cereal

rttr - https://github.com/rttrorg/rttr

spdlog - https://github.com/gabime/spdlog

tracey - https://github.com/r-lyeh/tracey

imgui - https://github.com/ocornut/imgui
