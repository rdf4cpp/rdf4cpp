cmake_minimum_required(VERSION 3.17)
project(rdf4cpp VERSION 0.0.4)

include(cmake/boilerplate_init.cmake)
boilerplate_init()

# find packages
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
find_package(serd 0 EXACT QUIET)

# find system libraries
if (NOT serd-FOUND)
    find_library(serd
            NAMES serd-0
            NO_CACHE REQUIRED)
    message(DEBUG "serd system library used: ${serd}")
else ()
    message(DEBUG "serd cmake package found")
endif ()

## Create the main rdf4cpp library target
add_library(rdf4cpp
        src/rdf4cpp/rdf/BlankNode.cpp
        src/rdf4cpp/rdf/Dataset.cpp
        src/rdf4cpp/rdf/Graph.cpp
        src/rdf4cpp/rdf/IRI.cpp
        src/rdf4cpp/rdf/Literal.cpp
        src/rdf4cpp/rdf/Node.cpp
        src/rdf4cpp/rdf/Quad.cpp
        src/rdf4cpp/rdf/query/QuadPattern.cpp
        src/rdf4cpp/rdf/query/Solution.cpp
        src/rdf4cpp/rdf/query/SolutionSequence.cpp
        src/rdf4cpp/rdf/query/TriplePattern.cpp
        src/rdf4cpp/rdf/query/Variable.cpp
        src/rdf4cpp/rdf/Statement.cpp
        src/rdf4cpp/rdf/storage/node/view/BNodeBackendView.cpp
        src/rdf4cpp/rdf/storage/node/view/IRIBackendView.cpp
        src/rdf4cpp/rdf/storage/node/view/LiteralBackendView.cpp
        src/rdf4cpp/rdf/storage/node/view/VariableBackendView.cpp
        src/rdf4cpp/rdf/storage/node/reference_node_storage/BNodeBackend.cpp
        src/rdf4cpp/rdf/storage/node/reference_node_storage/ReferenceNodeStorageBackend.cpp
        src/rdf4cpp/rdf/storage/node/reference_node_storage/IRIBackend.cpp
        src/rdf4cpp/rdf/storage/node/reference_node_storage/LiteralBackend.cpp
        src/rdf4cpp/rdf/storage/node/reference_node_storage/VariableBackend.cpp
        src/rdf4cpp/rdf/storage/node/identifier/LiteralType.cpp
        src/rdf4cpp/rdf/storage/node/identifier/NodeBackendHandle.cpp
        src/rdf4cpp/rdf/storage/node/identifier/NodeID.cpp
        src/rdf4cpp/rdf/storage/node/identifier/RDFNodeType.cpp
        src/rdf4cpp/rdf/storage/node/INodeStorageBackend.cpp
        src/rdf4cpp/rdf/storage/node/NodeStorage.cpp
        src/rdf4cpp/rdf/storage/tuple/DatasetStorage.cpp
        src/rdf4cpp/rdf/storage/tuple/DefaultDatasetBackend.cpp
        src/rdf4cpp/rdf/storage/tuple/DefaultSolutionSequenceBackend.cpp
        src/rdf4cpp/rdf/storage/tuple/IDatasetBackend.cpp
        src/rdf4cpp/rdf/storage/tuple/ISolutionSequenceBackend.cpp
        src/rdf4cpp/rdf/writer/NNodeWriter.cpp
        src/rdf4cpp/rdf/writer/NQuadsWriter.cpp
        src/rdf4cpp/rdf/writer/NTriplesWriter.cpp
        src/rdf4cpp/rdf/parser/SerdParsing.cpp
        )
add_library(rdf4cpp::rdf4cpp ALIAS rdf4cpp)

target_include_directories(
        rdf4cpp PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>")

target_link_libraries(rdf4cpp PRIVATE
        Threads::Threads
        $<IF:$<BOOL:serd-FOUND>,${serd},serd:serd> # ${VAR}-FOUND is only defined if the system library is used
        )

set_target_properties(rdf4cpp PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        CXX_STANDARD 20
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON)

if (IS_TOP_LEVEL)
    include(cmake/install_library.cmake)
    install_cpp_library(rdf4cpp src)
endif ()

if (BUILD_TESTING AND IS_TOP_LEVEL)
    message("Tests are configured to be build.")
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif ()

OPTION(BUILD_EXAMPLES "Build the examples for rdf4cpp." OFF)
if (BUILD_EXAMPLES AND IS_TOP_LEVEL)
    message("Examples are configured to be build.")
    add_subdirectory(examples)
endif ()