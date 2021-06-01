cmake_minimum_required(VERSION 3.17)
project(rdf4cpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -g -O0")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

add_library(rdf4cpp
        src/rdf4cpp/rdf/node/RDFNode.h
        src/rdf4cpp/rdf/node/RDFNode.cpp
        src/rdf4cpp/library.h
        src/rdf4cpp/utils/sec/Result.cpp
        src/rdf4cpp/utils/sec/Result.h
        src/rdf4cpp/utils/sec/Err.cpp
        src/rdf4cpp/utils/sec/Err.h
        src/rdf4cpp/utils/sec/Ok.cpp
        src/rdf4cpp/utils/sec/Ok.h
        src/rdf4cpp/rdf/node/ResourceNode.cpp
        src/rdf4cpp/rdf/node/ResourceNode.h
        src/rdf4cpp/rdf/node/IRIResource.cpp
        src/rdf4cpp/rdf/node/IRIResource.h
        src/rdf4cpp/rdf/node/BlankNode.cpp
        src/rdf4cpp/rdf/node/BlankNode.h
        src/rdf4cpp/rdf/node/Literal.cpp
        src/rdf4cpp/rdf/node/Literal.h
        src/rdf4cpp/library.cpp
        src/rdf4cpp/utils/IRI.cpp src/rdf4cpp/utils/IRI.h src/rdf4cpp/utils/error/InvalidIRIError.cpp src/rdf4cpp/utils/error/InvalidIRIError.h src/rdf4cpp/utils/utf8/UTF8String.cpp src/rdf4cpp/utils/utf8/UTF8String.h src/rdf4cpp/utils/error/InvalidUTF8StringError.cpp src/rdf4cpp/utils/error/InvalidUTF8StringError.h src/rdf4cpp/utils/utf8/UTF8StringStream.cpp src/rdf4cpp/utils/utf8/UTF8StringStream.h src/rdf4cpp/utils/utf8/UTF8Wrapper.cpp src/rdf4cpp/utils/utf8/UTF8Wrapper.h src/rdf4cpp/utils/StringValidator.cpp src/rdf4cpp/utils/StringValidator.h)
target_link_libraries(rdf4cpp PUBLIC
        rdfnode)
# target_include_directories(rdf4cpp PUBLIC src/rdf/node/)
add_executable(rdf4cpp_exec src/rdf4cpp/library.cpp)