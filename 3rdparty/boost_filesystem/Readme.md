
# Some minimal standalone boost libraries for embedding

Boost filesystem and iostreams and other core boost libraries bundled as embeddable CMake module.

Current boost version: **Boost 1.61**

Contains:

* boost::filesystem
* boost::iostreams
* boost::move
* boost::fusion
* boost::algorithm
* boost::range
* boost::optional
* boost::date_time

... and it's dependencies.

*****

Extracted with the bcp tool:

`bcp filesystem iostreams move optional fusion algorithm date_time range LOCATION`
*****

Easiest way to embed the minimal distribution in your project as git submodule using CMake:

Shell:
```sh
git submodule add https://github.com/Naios/boost-minimal-dist.git
```

CMake:
```cmake
add_subdirectory(boost-minimal-dist)

# Makes boost::filesystem and boost::iostreams available for your project
target_link_libraries(YOUR_PROJECT boost-minimal-dist)
```
