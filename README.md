⚠️ This repo is work-in-progress! Before v0.1.0 all APIs are considered unstable and might be subject to change. ⚠️
# rdf4cpp

rdf4cpp aims to be a stable, modern RDF library for C++.

## Usage 
check out the [examples](./examples) directory. 

### As Conan Package

Until its first stable release, rdf4cpp will not be available via Conan Center. Instead, it is available via the artifactory of the [DICE Research Group](https://dice-research.org/). 

You need the [package manager Conan](https://conan.io/downloads.html) installed and set up. You can add the DICE artifactory with:
```shell
conan remote add dice-group https://conan.dice-research.org/artifactory/api/conan/tentris
```

To use rdf4cpp, add it to your `conanfile.txt`:
```
[requires]
rdf4cpp/0.0.11
```

### With FetchContent
Use
```
include(FetchContent)
FetchContent_Declare(
        rdf4cpp
        GIT_REPOSITORY "${CMAKE_CURRENT_SOURCE_DIR}/../"
        GIT_TAG v0.0.11
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(rdf4cpp)
```

to make the library target `rdf4cpp::rdf4cpp` available. 

Beware: Conan will not be used for dependency retrieval if you include rdf4cpp via FetchContent. It is your responsibility that all dependencies are available  before. 

## Build

### Requirements

Currently, rdf4cpp builds only on linux with a C++20 compatible compiler. 
CI builds and tests rdf4cpp with gcc-{10,11}, clang-{12,13,14} with libstdc++-11 on ubuntu 22.04. 

### Dependencies

It is recommended to include build dependencies via conan. Set up Conan as follows on Ubuntu 20.04+:
```shell
sudo apt install python3-pip
pip3 install --user conan
conan user
conan profile new --detect default
conan profile update settings.compiler.libcxx=libstdc++11 default
```


### Compile
rdf4cpp uses CMake. To build it, run: 
```shell
cmake -B build_dir # configure and generate
cmake --build build_dir # compile
```

To install it to your system, run afterward:
```shell
sudo make install
```

### Additional CMake config options:

`-DBUILD_EXAMPLES=ON/OFF [default: OFF]`: Build the examples.

`-DBUILD_TESTING=ON/OFF [default: OFF]`: Build  the tests.

`-DBUILD_SHARED_LIBS=ON/OFF [default: OFF]`: Build a shared library instead of a static one.

`-DUSE_CONAN=ON/OFF [default: ON]`: If available, use Conan to retrieve dependencies.

