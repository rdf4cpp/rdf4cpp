cmake_minimum_required(VERSION 3.17)
project(rdf4cpp VERSION 0.0.11)

include(cmake/boilerplate_init.cmake)
boilerplate_init()

OPTION(USE_CONAN "If available, use conan to retrieve dependencies." ON)
if (IS_TOP_LEVEL AND USE_CONAN)
    include(cmake/conan_cmake.cmake)
    install_packages_via_conan("${CMAKE_SOURCE_DIR}/conanfile.py" "")
endif ()

# find packages
find_package(fmt 9 COMPONENTS fmt-header-only QUIET)
find_package(expected-lite 0.6 QUIET)
find_package(Boost REQUIRED)
find_package(re2 QUIET)
find_package(utf8cpp QUIET)

include(FetchContent)

# retrieve missing libraries
if (NOT fmt_FOUND)
    FetchContent_Declare(
            fetchfmt
            GIT_REPOSITORY https://github.com/fmtlib/fmt.git
            GIT_TAG c4ee726532178e556d923372f29163bd206d7732 # v9.0.0
    )

    FetchContent_MakeAvailable(fetchfmt)
    message(DEBUG "No CMake package for fmt found. fmt was retrieved via FetchContent.")
endif ()

if (NOT expected-lite_FOUND)
    FetchContent_Declare(
            expected-lite
            GIT_REPOSITORY https://github.com/martinmoene/expected-lite
            GIT_TAG 95b9cb015fa17baa749c2b396b335906e1596a9e # v0.6.2
    )

    FetchContent_MakeAvailable(expected-lite)
    message(DEBUG "No CMake package for expected-lite found. expected-lite was retrieved via FetchContent.")
endif ()

if (NOT re2_FOUND)
    FetchContent_Declare(
            fetchre2
            GIT_REPOSITORY https://github.com/google/re2
            GIT_TAG 2022-12-01
    )

    set(RE2_BUILD_TESTING OFF CACHE INTERNAL "")
    FetchContent_MakeAvailable(fetchre2)
    message(DEBUG "No CMake package for re2 found. re2 was retrieved via FetchContent.")
endif ()

if (NOT utf8cpp_FOUND)
    FetchContent_Declare(
            fetchutfcpp
            GIT_REPOSITORY https://github.com/nemtrif/utfcpp
            GIT_TAG v3.2.3
    )

    FetchContent_MakeAvailable(fetchutfcpp)
    message(DEBUG "No CMake package for utfcpp found. utfcpp was retrieved via FetchContent.")
endif ()

set(serd_source_files
        include/serd/serd.h
        src/attributes.h
        src/base64.c
        src/base64.h
        src/byte_sink.h
        src/byte_source.c
        src/byte_source.h
        src/env.c
        src/n3.c
        src/node.c
        src/node.h
        src/reader.c
        src/reader.h
        src/serd_config.h
        src/serd_internal.h
        src/stack.h
        src/string.c
        src/string_utils.h
        src/system.c
        src/system.h
        src/uri.c
        src/uri_utils.h
        src/writer.c
)

foreach(serd_source_file ${serd_source_files})
    file(DOWNLOAD "https://raw.githubusercontent.com/dice-group/serd/95f5929c06a85495513fceee568a08c3cafaae83/${serd_source_file}"
            "${CMAKE_CURRENT_BINARY_DIR}/serd/${serd_source_file}"
            TLS_VERIFY ON)
endforeach()
file(DOWNLOAD "https://raw.githubusercontent.com/dice-group/serd/36cd3b34bd7e0793b13aa37b1a3d7e67854c4807/COPYING"
        "${CMAKE_CURRENT_BINARY_DIR}/serd/COPYING"
        TLS_VERIFY ON)

list(FILTER serd_source_files INCLUDE REGEX "^.+\\.c$")
list(TRANSFORM serd_source_files PREPEND "${CMAKE_CURRENT_BINARY_DIR}/serd/")

## Create the main rdf4cpp library target
add_library(rdf4cpp
        src/rdf4cpp/rdf/BlankNode.cpp
        src/rdf4cpp/rdf/ClosedNamespace.cpp
        src/rdf4cpp/rdf/Dataset.cpp
        src/rdf4cpp/rdf/Graph.cpp
        src/rdf4cpp/rdf/IRI.cpp
        src/rdf4cpp/rdf/Literal.cpp
        src/rdf4cpp/rdf/Namespace.cpp
        src/rdf4cpp/rdf/Node.cpp
        src/rdf4cpp/rdf/Quad.cpp
        src/rdf4cpp/rdf/Statement.cpp
        src/rdf4cpp/rdf/datatypes/registry/DatatypeRegistry.cpp
        src/rdf4cpp/rdf/datatypes/owl/Rational.cpp
        src/rdf4cpp/rdf/datatypes/owl/Real.cpp
        src/rdf4cpp/rdf/datatypes/rdf/LangString.cpp
        src/rdf4cpp/rdf/datatypes/xsd/Base64Binary.cpp
        src/rdf4cpp/rdf/datatypes/xsd/Boolean.cpp
        src/rdf4cpp/rdf/datatypes/xsd/Decimal.cpp
        src/rdf4cpp/rdf/datatypes/xsd/Double.cpp
        src/rdf4cpp/rdf/datatypes/xsd/Float.cpp
        src/rdf4cpp/rdf/datatypes/xsd/HexBinary.cpp
        src/rdf4cpp/rdf/datatypes/xsd/String.cpp
        src/rdf4cpp/rdf/datatypes/xsd/integers/non_negative/NonNegativeInteger.cpp
        src/rdf4cpp/rdf/datatypes/xsd/integers/non_negative/PositiveInteger.cpp
        src/rdf4cpp/rdf/datatypes/xsd/integers/non_negative/UnsignedByte.cpp
        src/rdf4cpp/rdf/datatypes/xsd/integers/non_negative/UnsignedInt.cpp
        src/rdf4cpp/rdf/datatypes/xsd/integers/non_negative/UnsignedLong.cpp
        src/rdf4cpp/rdf/datatypes/xsd/integers/non_negative/UnsignedShort.cpp
        src/rdf4cpp/rdf/datatypes/xsd/integers/non_positive/NegativeInteger.cpp
        src/rdf4cpp/rdf/datatypes/xsd/integers/non_positive/NonPositiveInteger.cpp
        src/rdf4cpp/rdf/datatypes/xsd/integers/signed/Byte.cpp
        src/rdf4cpp/rdf/datatypes/xsd/integers/signed/Int.cpp
        src/rdf4cpp/rdf/datatypes/xsd/integers/signed/Integer.cpp
        src/rdf4cpp/rdf/datatypes/xsd/integers/signed/Long.cpp
        src/rdf4cpp/rdf/datatypes/xsd/integers/signed/Short.cpp
        src/rdf4cpp/rdf/namespaces/BFLC.cpp
        src/rdf4cpp/rdf/namespaces/FOAF.cpp
        src/rdf4cpp/rdf/namespaces/MADSRDF.cpp
        src/rdf4cpp/rdf/namespaces/OWL.cpp
        src/rdf4cpp/rdf/namespaces/PREMIS3.cpp
        src/rdf4cpp/rdf/namespaces/RDF.cpp
        src/rdf4cpp/rdf/namespaces/RDFS.cpp
        src/rdf4cpp/rdf/namespaces/XSD.cpp
        src/rdf4cpp/rdf/parser/IStreamQuadIterator.cpp
        src/rdf4cpp/rdf/parser/RDFFileParser.cpp
        src/rdf4cpp/rdf/query/QuadPattern.cpp
        src/rdf4cpp/rdf/query/Solution.cpp
        src/rdf4cpp/rdf/query/SolutionSequence.cpp
        src/rdf4cpp/rdf/query/TriplePattern.cpp
        src/rdf4cpp/rdf/query/Variable.cpp
        src/rdf4cpp/rdf/regex/Regex.cpp
        src/rdf4cpp/rdf/regex/RegexReplacer.cpp
        src/rdf4cpp/rdf/storage/node/INodeStorageBackend.cpp
        src/rdf4cpp/rdf/storage/node/NodeStorage.cpp
        src/rdf4cpp/rdf/storage/node/identifier/LiteralType.cpp
        src/rdf4cpp/rdf/storage/node/identifier/NodeBackendHandle.cpp
        src/rdf4cpp/rdf/storage/node/identifier/NodeID.cpp
        src/rdf4cpp/rdf/storage/node/identifier/RDFNodeType.cpp
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
        private/rdf4cpp/rdf/parser/IStreamQuadIteratorSerdImpl.cpp
        private/rdf4cpp/rdf/regex/RegexImpl.cpp
        private/rdf4cpp/rdf/regex/RegexReplacerImpl.cpp
        ${serd_source_files}
)

add_library(rdf4cpp::rdf4cpp ALIAS rdf4cpp)

target_include_directories(
        rdf4cpp
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
        PRIVATE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/private>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/serd/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/serd/src>"
)

target_link_libraries(rdf4cpp
        PUBLIC
        nonstd::expected-lite
        Boost::headers
        PRIVATE
        fmt::fmt-header-only
        re2::re2
        utf8cpp
)

set_target_properties(rdf4cpp PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        CXX_STANDARD 20
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
)

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
