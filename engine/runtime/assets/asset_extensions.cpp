#include "asset_extensions.h"

const std::array<std::string, 6> extensions::texture =
{
    ".png", ".jpg", ".tga", ".dds", ".ktx", ".pvr"
};
const std::array<std::string, 5> extensions::mesh =
{
    ".obj", ".fbx", ".dae", ".blend", ".3ds"
};
std::string extensions::shader = ".sc";
std::string extensions::material = ".mat";
std::string extensions::prefab = ".pfb";
std::string extensions::scene = ".sgr";
std::string extensions::compiled = ".asset";