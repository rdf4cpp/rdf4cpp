cmake_minimum_required(VERSION 3.17)
project(rdf4cpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER clang++)


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
        )
target_link_libraries(rdf4cpp PUBLIC
        rdfnode)
# target_include_directories(rdf4cpp PUBLIC src/rdf/node/)
add_executable(rdf4cpp_exec src/rdf4cpp/library.cpp)