cmake_minimum_required(VERSION 3.17)

add_library(rdf4cpp_datatypes OBJECT
        owl/Rational.cpp
        owl/Real.cpp
        rdf/LangString.cpp
        xsd/integers/non_positive/NegativeInteger.cpp
        xsd/integers/non_positive/NonPositiveInteger.cpp
        xsd/integers/non_negative/NonNegativeInteger.cpp
        xsd/integers/non_negative/PositiveInteger.cpp
        xsd/integers/non_negative/UnsignedByte.cpp
        xsd/integers/non_negative/UnsignedInt.cpp
        xsd/integers/non_negative/UnsignedLong.cpp
        xsd/integers/non_negative/UnsignedShort.cpp
        xsd/integers/non_positive/NegativeInteger.cpp
        xsd/integers/non_positive/NonPositiveInteger.cpp
        xsd/integers/signed/Byte.cpp
        xsd/integers/signed/Int.cpp
        xsd/integers/signed/Integer.cpp
        xsd/integers/signed/Long.cpp
        xsd/integers/signed/Short.cpp
        xsd/Boolean.cpp
        xsd/Decimal.cpp
        xsd/Double.cpp
        xsd/Float.cpp
        xsd/String.cpp
        )

target_include_directories(
        rdf4cpp_datatypes
        PRIVATE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/../../.."
        )

target_link_libraries(rdf4cpp_datatypes
        PUBLIC
        nonstd::expected-lite
        Boost::headers
        )

set_target_properties(rdf4cpp_datatypes PROPERTIES
        CXX_STANDARD 20
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
        POSITION_INDEPENDENT_CODE ON)
