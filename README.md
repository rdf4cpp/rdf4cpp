⚠️ This repo is work-in-progress! Before v0.1.0 all APIs are considered unstable and might be subject to change. ⚠️
# rdf4cpp

rdf4cpp aims to be a stable, modern RDF library for C++.

## Usage 
check out the [examples](./examples) directory. 

## Requirements

Currently, rdf4cpp builds only on linux with a C++20 compatible compiler. 
Tests are build with gcc 10 and clang 10. 
Newer version of the compilers should also work. 

To load RDF data from files, you need additionally the library [serd](https://drobilla.net/software/serd). 
It is available on most distributions package managers. 
On Ubuntu, install it with 
```shell
sudo apt install libserd-dev
```

## Build
rdf4cpp uses CMake. To build it, run: 
```shell
cmake -B build_dir # configure and generate
cmake --build build_dir # compile
```

To install it to your system, run afterwards:
```shell
sudo make install
```

### Additional CMake config cptions:

`-DBUILD_EXAMPLES=ON/OFF [default: OFF]`: configure cmake for building examples

`-DBUILD_TESTING=ON/OFF [default: OFF]`: configure cmake for building tests

`-DBUILD_SHARED_LIBS=ON/OFF [default: OFF]`: build a shared library instead of a static