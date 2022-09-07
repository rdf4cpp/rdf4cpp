cmake_minimum_required(VERSION 3.17)
project(rdf4cpp VERSION 0.0.5)

include(cmake/boilerplate_init.cmake)
boilerplate_init()

OPTION(USE_CONAN "If available, use conan to retrieve dependencies." ON)
if (IS_TOP_LEVEL AND USE_CONAN)
    include(cmake/conan_cmake.cmake)
    install_packages_via_conan("${CMAKE_SOURCE_DIR}/conanfile.py" "")
endif ()

# find packages
find_package(serd 0 QUIET)
find_package(fmt 9 COMPONENTS fmt-header-only QUIET)
find_package(expected-lite)

# find system libraries
if (NOT serd_FOUND)
    find_library(serd
            NAMES serd-0
            NO_CACHE REQUIRED)
    message(DEBUG "serd system library used: ${serd}")
else ()
    message(DEBUG "serd cmake package found")
endif ()

include(FetchContent)

# retrieve missing libraries
if (NOT fmt_FOUND)
    FetchContent_Declare(
            fetchfmt
            GIT_REPOSITORY https://github.com/fmtlib/fmt.git
            GIT_TAG c4ee726532178e556d923372f29163bd206d7732 # v9.0.0
    )

    FetchContent_MakeAvailable(fetchfmt)
    message(DEBUG "Non CMake package for fmt found. fmt was retrieved via FetchContent.")
endif ()

if (NOT expected-lite_FOUND)
    FetchContent_Declare(
            fetchexpected-lite
            GIT_REPOSITORY https://github.com/martinmoene/expected-lite
            GIT_TAG v0.6.2
    )

    FetchContent_MakeAvailable(fetchexpected-lite)
    message(DEBUG "No CMake package for expected-lite found. expected-lite was retrieved via FetchContent.")
endif ()

## Create the main rdf4cpp library target
add_library(rdf4cpp
        src/rdf4cpp/rdf/BlankNode.cpp
        src/rdf4cpp/rdf/ClosedNamespace.cpp
        src/rdf4cpp/rdf/Dataset.cpp
        src/rdf4cpp/rdf/Graph.cpp
        src/rdf4cpp/rdf/IRI.cpp
        src/rdf4cpp/rdf/Literal.cpp
        src/rdf4cpp/rdf/Namespace.cpp
        src/rdf4cpp/rdf/namespaces/BFLC.cpp
        src/rdf4cpp/rdf/namespaces/FOAF.cpp
        src/rdf4cpp/rdf/namespaces/MADSRDF.cpp
        src/rdf4cpp/rdf/namespaces/OWL.cpp
        src/rdf4cpp/rdf/namespaces/PREMIS3.cpp
        src/rdf4cpp/rdf/namespaces/RDF.cpp
        src/rdf4cpp/rdf/namespaces/RDFS.cpp
        src/rdf4cpp/rdf/namespaces/XSD.cpp
        src/rdf4cpp/rdf/Node.cpp
        src/rdf4cpp/rdf/parser/SerdParsing.cpp
        src/rdf4cpp/rdf/Quad.cpp
        src/rdf4cpp/rdf/query/QuadPattern.cpp
        src/rdf4cpp/rdf/query/Solution.cpp
        src/rdf4cpp/rdf/query/SolutionSequence.cpp
        src/rdf4cpp/rdf/query/TriplePattern.cpp
        src/rdf4cpp/rdf/query/Variable.cpp
        src/rdf4cpp/rdf/Statement.cpp
        src/rdf4cpp/rdf/storage/node/identifier/LiteralType.cpp
        src/rdf4cpp/rdf/storage/node/identifier/NodeBackendHandle.cpp
        src/rdf4cpp/rdf/storage/node/identifier/NodeID.cpp
        src/rdf4cpp/rdf/storage/node/identifier/RDFNodeType.cpp
        src/rdf4cpp/rdf/storage/node/INodeStorageBackend.cpp
        src/rdf4cpp/rdf/storage/node/NodeStorage.cpp
        src/rdf4cpp/rdf/storage/node/reference_node_storage/BNodeBackend.cpp
        src/rdf4cpp/rdf/storage/node/reference_node_storage/IRIBackend.cpp
        src/rdf4cpp/rdf/storage/node/reference_node_storage/LiteralBackend.cpp
        src/rdf4cpp/rdf/storage/node/reference_node_storage/ReferenceNodeStorageBackend.cpp
        src/rdf4cpp/rdf/storage/node/reference_node_storage/VariableBackend.cpp
        src/rdf4cpp/rdf/storage/node/view/BNodeBackendView.cpp
        src/rdf4cpp/rdf/storage/node/view/IRIBackendView.cpp
        src/rdf4cpp/rdf/storage/node/view/LiteralBackendView.cpp
        src/rdf4cpp/rdf/storage/node/view/VariableBackendView.cpp
        src/rdf4cpp/rdf/storage/tuple/DatasetStorage.cpp
        src/rdf4cpp/rdf/storage/tuple/DefaultDatasetBackend.cpp
        src/rdf4cpp/rdf/storage/tuple/DefaultSolutionSequenceBackend.cpp
        src/rdf4cpp/rdf/storage/tuple/IDatasetBackend.cpp
        src/rdf4cpp/rdf/storage/tuple/ISolutionSequenceBackend.cpp
        src/rdf4cpp/rdf/writer/NNodeWriter.cpp
        src/rdf4cpp/rdf/writer/NQuadsWriter.cpp
        src/rdf4cpp/rdf/writer/NTriplesWriter.cpp
        )
add_library(rdf4cpp::rdf4cpp ALIAS rdf4cpp)

target_include_directories(
        rdf4cpp PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
        "${expected-lite_INCLUDE_DIRS}")

target_link_libraries(rdf4cpp PRIVATE
        $<IF:${serd_FOUND},serd::serd,${serd}> # ${VAR}_FOUND is only defined if the system library is used
        fmt::fmt-header-only
        nonstd::expected-lite)

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