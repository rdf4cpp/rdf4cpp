cmake_minimum_required(VERSION 3.22)
project(rdf4cpp VERSION 0.0.27)

include(cmake/boilerplate_init.cmake)
boilerplate_init()

OPTION(USE_CONAN "If available, use conan to retrieve dependencies." ON)
if (PROJECT_IS_TOP_LEVEL AND USE_CONAN)
    include(cmake/conan_cmake.cmake)
    if (BUILD_TESTING)
        set(CONAN_HYPERTRIE_WITH_TEST_DEPS "True")
    else()
        set(CONAN_HYPERTRIE_WITH_TEST_DEPS "False")
    endif()
    set(CONAN_OPTIONS "with_test_deps=${CONAN_HYPERTRIE_WITH_TEST_DEPS}")
    install_packages_via_conan("${CMAKE_SOURCE_DIR}/conanfile.py" "${CONAN_OPTIONS};boost/*:header_only=True")
endif ()

# find packages
find_package(expected-lite QUIET)
find_package(Boost REQUIRED)
find_package(re2 QUIET)
find_package(OpenSSL REQUIRED)
find_package(uni-algo QUIET)
find_package(dice-hash QUIET)
find_package(dice-sparse-map QUIET)
find_package(dice-template-library QUIET)

include(FetchContent)

if (NOT expected-lite_FOUND)
    FetchContent_Declare(
            fetchexpected-lite
            GIT_REPOSITORY https://github.com/martinmoene/expected-lite
            GIT_TAG 95b9cb015fa17baa749c2b396b335906e1596a9e # v0.6.2
    )

    FetchContent_MakeAvailable(fetchexpected-lite)
    message(DEBUG "No CMake package for expected-lite found. expected-lite was retrieved via FetchContent.")
endif ()

if (NOT re2_FOUND)
    FetchContent_Declare(
            fetchre2
            GIT_REPOSITORY https://github.com/google/re2
            GIT_TAG 4be240789d5b322df9f02b7e19c8651f3ccbf205 # 2022-12-01
    )

    set(RE2_BUILD_TESTING OFF CACHE INTERNAL "")
    FetchContent_MakeAvailable(fetchre2)
    message(DEBUG "No CMake package for re2 found. re2 was retrieved via FetchContent.")
endif ()

if (NOT uni-algo_FOUND)
    set(UNI_ALGO_INSTALL On)
    FetchContent_Declare(
            fetchuni-algo
            GIT_REPOSITORY https://github.com/uni-algo/uni-algo.git
            GIT_SHALLOW ON
            GIT_TAG 00cfcbfcbe9720760f8156ee086fc8f45ae59ae4 # v1.2.0
    )
    FetchContent_MakeAvailable(fetchuni-algo)
endif ()

if (NOT dice-hash_FOUND)
    FetchContent_Declare(
            fetchdice-hash
            GIT_REPOSITORY https://github.com/dice-group/dice-hash.git
            GIT_TAG v0.4.3
            GIT_SHALLOW TRUE)

    FetchContent_MakeAvailable(fetchdice-hash)
endif ()

if (NOT dice-sparse-map_FOUND)
    FetchContent_Declare(
            fetchdice-sparse-map
            GIT_REPOSITORY https://github.com/dice-group/dice-sparse-map.git
            GIT_TAG v0.2.4
            GIT_SHALLOW TRUE)

    FetchContent_MakeAvailable(fetchdice-sparse-map)
endif ()

if (NOT dice-template-library_FOUND)
    FetchContent_Declare(
            fetchdice-template-library
            GIT_REPOSITORY https://github.com/dice-group/dice-template-library.git
            GIT_TAG v1.4.0
            GIT_SHALLOW TRUE)

    FetchContent_MakeAvailable(fetchdice-template-library)
endif ()


configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.hpp.in ${CMAKE_CURRENT_SOURCE_DIR}/src/rdf4cpp/version.hpp)

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
        src/try.h
        src/uri.c
        src/uri_utils.h
        src/writer.c
)

foreach(serd_source_file ${serd_source_files})
    file(DOWNLOAD "https://raw.githubusercontent.com/dice-group/serd/4352d002c55322fbc1a8baca6b0af6afa0142f5b/${serd_source_file}"
            "${CMAKE_CURRENT_BINARY_DIR}/serd/${serd_source_file}"
            TLS_VERIFY ON)
endforeach()
file(DOWNLOAD "https://raw.githubusercontent.com/dice-group/serd/4352d002c55322fbc1a8baca6b0af6afa0142f5b/COPYING"
        "${CMAKE_CURRENT_BINARY_DIR}/serd/COPYING"
        TLS_VERIFY ON)

list(FILTER serd_source_files INCLUDE REGEX "^.+\\.c$")
list(TRANSFORM serd_source_files PREPEND "${CMAKE_CURRENT_BINARY_DIR}/serd/")

## Create the main rdf4cpp library target
add_library(rdf4cpp
        src/rdf4cpp/BlankNode.cpp
        src/rdf4cpp/ClosedNamespace.cpp
        src/rdf4cpp/Dataset.cpp
        src/rdf4cpp/Graph.cpp
        src/rdf4cpp/IRI.cpp
        src/rdf4cpp/Literal.cpp
        src/rdf4cpp/Namespace.cpp
        src/rdf4cpp/Node.cpp
        src/rdf4cpp/Quad.cpp
        src/rdf4cpp/Statement.cpp
        src/rdf4cpp/bnode_mngt/NodeGenerator.cpp
        src/rdf4cpp/bnode_mngt/NodeScope.cpp
        src/rdf4cpp/bnode_mngt/WeakNodeScope.cpp
        src/rdf4cpp/bnode_mngt/reference_backends/factory/SkolemIRIFactory.cpp
        src/rdf4cpp/bnode_mngt/reference_backends/factory/BNodeFactory.cpp
        src/rdf4cpp/bnode_mngt/reference_backends/generator/RandomIdGenerator.cpp
        src/rdf4cpp/bnode_mngt/reference_backends/generator/IncreasingIdGenerator.cpp
        src/rdf4cpp/bnode_mngt/reference_backends/scope/ReferenceNodeScope.cpp
        src/rdf4cpp/bnode_mngt/reference_backends/scope_manager/ReferenceNodeScopeManager.cpp
        src/rdf4cpp/datatypes/registry/DatatypeRegistry.cpp
        src/rdf4cpp/datatypes/owl/Rational.cpp
        src/rdf4cpp/datatypes/owl/Real.cpp
        src/rdf4cpp/datatypes/rdf/LangString.cpp
        src/rdf4cpp/datatypes/xsd/Base64Binary.cpp
        src/rdf4cpp/datatypes/xsd/Boolean.cpp
        src/rdf4cpp/datatypes/xsd/Decimal.cpp
        src/rdf4cpp/datatypes/xsd/Double.cpp
        src/rdf4cpp/datatypes/xsd/Float.cpp
        src/rdf4cpp/datatypes/xsd/HexBinary.cpp
        src/rdf4cpp/datatypes/xsd/String.cpp
        src/rdf4cpp/datatypes/xsd/integers/non_negative/NonNegativeInteger.cpp
        src/rdf4cpp/datatypes/xsd/integers/non_negative/PositiveInteger.cpp
        src/rdf4cpp/datatypes/xsd/integers/non_negative/UnsignedByte.cpp
        src/rdf4cpp/datatypes/xsd/integers/non_negative/UnsignedInt.cpp
        src/rdf4cpp/datatypes/xsd/integers/non_negative/UnsignedLong.cpp
        src/rdf4cpp/datatypes/xsd/integers/non_negative/UnsignedShort.cpp
        src/rdf4cpp/datatypes/xsd/integers/non_positive/NegativeInteger.cpp
        src/rdf4cpp/datatypes/xsd/integers/non_positive/NonPositiveInteger.cpp
        src/rdf4cpp/datatypes/xsd/integers/signed/Byte.cpp
        src/rdf4cpp/datatypes/xsd/integers/signed/Int.cpp
        src/rdf4cpp/datatypes/xsd/integers/signed/Integer.cpp
        src/rdf4cpp/datatypes/xsd/integers/signed/Long.cpp
        src/rdf4cpp/datatypes/xsd/integers/signed/Short.cpp
        src/rdf4cpp/datatypes/xsd/time/Year.cpp
        src/rdf4cpp/datatypes/xsd/time/Month.cpp
        src/rdf4cpp/datatypes/xsd/time/Day.cpp
        src/rdf4cpp/datatypes/xsd/time/YearMonth.cpp
        src/rdf4cpp/datatypes/xsd/time/MonthDay.cpp
        src/rdf4cpp/datatypes/xsd/time/Date.cpp
        src/rdf4cpp/datatypes/xsd/time/Time.cpp
        src/rdf4cpp/datatypes/xsd/time/DateTime.cpp
        src/rdf4cpp/datatypes/xsd/time/DateTimeStamp.cpp
        src/rdf4cpp/datatypes/xsd/time/Duration.cpp
        src/rdf4cpp/datatypes/xsd/time/DayTimeDuration.cpp
        src/rdf4cpp/datatypes/xsd/time/YearMonthDuration.cpp
        src/rdf4cpp/namespaces/RDF.cpp
        src/rdf4cpp/parser/IStreamQuadIterator.cpp
        src/rdf4cpp/parser/RDFFileParser.cpp
        src/rdf4cpp/query/QuadPattern.cpp
        src/rdf4cpp/query/Solution.cpp
        src/rdf4cpp/query/TriplePattern.cpp
        src/rdf4cpp/query/Variable.cpp
        src/rdf4cpp/regex/Regex.cpp
        src/rdf4cpp/regex/RegexReplacer.cpp
        src/rdf4cpp/storage/NodeStorage.cpp
        src/rdf4cpp/storage/reference_node_storage/SyncReferenceNodeStorage.cpp
        src/rdf4cpp/storage/reference_node_storage/UnsyncReferenceNodeStorage.cpp
        src/rdf4cpp/storage/view/BNodeBackendView.cpp
        src/rdf4cpp/storage/view/IRIBackendView.cpp
        src/rdf4cpp/storage/view/LiteralBackendView.cpp
        src/rdf4cpp/storage/view/VariableBackendView.cpp
        src/rdf4cpp/writer/SerializationState.cpp
        src/rdf4cpp/IRIView.cpp
        src/rdf4cpp/IRIFactory.cpp
        private/rdf4cpp/parser/IStreamQuadIteratorSerdImpl.cpp
        private/rdf4cpp/regex/RegexImpl.cpp
        private/rdf4cpp/regex/RegexReplacerImpl.cpp
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
        dice-hash::dice-hash
        dice-sparse-map::dice-sparse-map
        dice-template-library::dice-template-library
        PRIVATE
        re2::re2
        OpenSSL::Crypto
        uni-algo::uni-algo
        )

set_target_properties(rdf4cpp PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        CXX_STANDARD 20
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
)

if (PROJECT_IS_TOP_LEVEL)
    include(cmake/install_library.cmake)
    install_cpp_library(rdf4cpp src)
endif ()

if (PROJECT_IS_TOP_LEVEL AND BUILD_TESTING)
    message("Tests are configured to be build.")
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif ()

OPTION(BUILD_EXAMPLES "Build the examples for rdf4cpp." OFF)
if (PROJECT_IS_TOP_LEVEL AND BUILD_EXAMPLES)
    message("Examples are configured to be build.")
    add_subdirectory(examples)
endif ()
