cmake_minimum_required(VERSION 3.17)
project(rdf4cpp VERSION 0.0.1)
include(CTest)

## C++ compiler flags

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage -fPIC -g -O0")
endif ()

## C++ language configuration boilerplate

if (NOT DEFINED CMAKE_CXX_VISIBILITY_PRESET AND
        NOT DEFINED CMAKE_VISIBILITY_INLINES_HIDDEN)
    set(CMAKE_CXX_VISIBILITY_PRESET default)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN NO)
endif ()

## Let rdf4cpp_SHARED_LIBS override BUILD_SHARED_LIBS

if (DEFINED rdf4cpp_SHARED_LIBS)
    set(BUILD_SHARED_LIBS "${rdf4cpp_SHARED_LIBS}")
endif ()

# find packages
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

# TODO: check version of serd
find_library(serd
        NAMES serd-0
        NO_CACHE)

## Create the main rdf4cpp library target

add_library(rdf4cpp
        src/rdf4cpp/rdf/BlankNode.cpp
        src/rdf4cpp/rdf/Dataset.cpp
        src/rdf4cpp/rdf/Graph.cpp
        src/rdf4cpp/rdf/IRI.cpp
        src/rdf4cpp/rdf/Literal.cpp
        src/rdf4cpp/rdf/Node.cpp
        src/rdf4cpp/rdf/Quad.cpp
        src/rdf4cpp/rdf/query/Solution.cpp
        src/rdf4cpp/rdf/query/SolutionSequence.cpp
        src/rdf4cpp/rdf/query/QuadPattern.cpp
        src/rdf4cpp/rdf/query/TriplePattern.cpp
        src/rdf4cpp/rdf/query/Variable.cpp
        src/rdf4cpp/rdf/Statement.cpp
        src/rdf4cpp/rdf/storage/node/BackendNodeHandle.cpp
        src/rdf4cpp/rdf/storage/node/BNodeBackend.cpp
        src/rdf4cpp/rdf/storage/node/DefaultNodeStorageBackend.cpp
        src/rdf4cpp/rdf/storage/node/INodeStorageBackend.cpp
        src/rdf4cpp/rdf/storage/node/IRIBackend.cpp
        src/rdf4cpp/rdf/storage/node/LiteralBackend.cpp
        src/rdf4cpp/rdf/storage/node/NodeID.cpp
        src/rdf4cpp/rdf/storage/node/NodeStorage.cpp
        src/rdf4cpp/rdf/storage/node/RDFNodeType.cpp
        src/rdf4cpp/rdf/storage/node/VariableBackend.cpp
        src/rdf4cpp/rdf/storage/tuple/DatasetStorage.cpp
        src/rdf4cpp/rdf/storage/tuple/DefaultDatasetBackend.cpp
        src/rdf4cpp/rdf/storage/tuple/DefaultSolutionSequenceBackend.cpp
        src/rdf4cpp/rdf/storage/tuple/IDatasetBackend.cpp
        src/rdf4cpp/rdf/storage/tuple/ISolutionSequenceBackend.cpp
        src/rdf4cpp/rdf/writer/NTriplesWriter.cpp
        src/rdf4cpp/rdf/writer/NQuadsWriter.cpp
        src/rdf4cpp/rdf/writer/NNodeWriter.cpp
        src/rdf4cpp/utils/error/InvalidIRIError.cpp
        src/rdf4cpp/utils/error/InvalidUTF8StringError.cpp
        src/rdf4cpp/utils/PlainIRI.cpp
        src/rdf4cpp/utils/sec/Err.cpp
        src/rdf4cpp/utils/sec/Ok.cpp
        src/rdf4cpp/utils/sec/Result.cpp
        src/rdf4cpp/utils/StringValidator.cpp
        src/rdf4cpp/utils/utf8/UTF8String.cpp
        src/rdf4cpp/utils/utf8/UTF8StringStream.cpp
        src/rdf4cpp/utils/utf8/UTF8Wrapper.cpp
        $<$<NOT:$<STREQUAL:"${serd}","serd-NOTFOUND">>:src/rdf4cpp/rdf/parser/SerdParsing.cpp>
        )
target_link_libraries(rdf4cpp PRIVATE
        Threads::Threads
        $<$<NOT:$<STREQUAL:"${serd}","serd-NOTFOUND">>:${serd}>
        )
add_library(rdf4cpp::rdf4cpp ALIAS rdf4cpp)
set_target_properties(rdf4cpp PROPERTIES
        VERSION ${rdf4cpp_VERSION}
        SOVERSION ${rdf4cpp_VERSION_MAJOR})
target_include_directories(
        rdf4cpp PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>")
target_compile_features(rdf4cpp PUBLIC cxx_std_20)

## Generate the export header for rdf4cpp and attach it to the target

include(GenerateExportHeader)
generate_export_header(rdf4cpp EXPORT_FILE_NAME include/RDF4CPP/export.h)
target_compile_definitions(
        rdf4cpp PUBLIC "$<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:RDF4CPP_STATIC_DEFINE>")
target_include_directories(
        rdf4cpp PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>")

## Include the install rules if the user wanted them (included by default when top-level)

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" is_top_level)
option(rdf4cpp_INCLUDE_PACKAGING "Include packaging rules for rdf4cpp" "${is_top_level}")
if (rdf4cpp_INCLUDE_PACKAGING)
    add_subdirectory(packaging)
endif ()

OPTION(rdf4cpp_BUILD_TESTS "Build the tests for rdf4cpp." OFF)
if (rdf4cpp_BUILD_TESTS OR BUILD_TESTING)
    message("Tests are configured to be build.")
    add_subdirectory(tests)
endif ()


OPTION(rdf4cpp_BUILD_EXAMPLES "Build the examples for rdf4cpp." OFF)
if (rdf4cpp_BUILD_EXAMPLES)
    message("Examples are configured to be build.")
    add_subdirectory(examples)
endif ()