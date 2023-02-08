set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
        DocTest
        GIT_REPOSITORY "https://github.com/onqtam/doctest"
        GIT_TAG  "v2.4.8"
)
FetchContent_MakeAvailable(DocTest)

# add the executable for all tests
add_executable(tests_Variable query/tests_Variable.cpp)
target_link_libraries(tests_Variable
        doctest
        rdf4cpp
        )
set_property(TARGET tests_Variable PROPERTY CXX_STANDARD 20)
add_test(NAME tests_Variable COMMAND tests_Variable)


add_executable(tests_TriplePattern query/tests_TriplePattern.cpp)
target_link_libraries(tests_TriplePattern
        doctest
        rdf4cpp
        )
set_property(TARGET tests_TriplePattern PROPERTY CXX_STANDARD 20)
add_test(NAME tests_TriplePattern COMMAND tests_TriplePattern)


add_executable(tests_QuadPattern query/tests_QuadPattern.cpp)
target_link_libraries(tests_QuadPattern
        doctest
        rdf4cpp
        )
set_property(TARGET tests_QuadPattern PROPERTY CXX_STANDARD 20)
add_test(NAME tests_QuadPattern COMMAND tests_QuadPattern)


add_executable(tests_Literal nodes/tests_Literal.cpp)
target_link_libraries(tests_Literal
        doctest
        rdf4cpp
        )
set_property(TARGET tests_Literal PROPERTY CXX_STANDARD 20)
add_test(NAME tests_Literal COMMAND tests_Literal)

add_executable(tests_Literal_ops nodes/tests_Literal_ops.cpp)
target_link_libraries(tests_Literal_ops
        doctest
        rdf4cpp
        )
set_property(TARGET tests_Literal_ops PROPERTY CXX_STANDARD 20)
add_test(NAME tests_Literal_ops COMMAND tests_Literal_ops)

add_executable(tests_Node nodes/tests_Node.cpp)
target_link_libraries(tests_Node
        doctest
        rdf4cpp
        )
set_property(TARGET tests_Node PROPERTY CXX_STANDARD 20)
add_test(NAME tests_Node COMMAND tests_Node)

# RDF Core Types
add_executable(tests_String datatype/tests_String.cpp)
target_link_libraries(tests_String
        doctest
        rdf4cpp
        )
set_property(TARGET tests_String PROPERTY CXX_STANDARD 20)
add_test(NAME tests_String COMMAND tests_String)

add_executable(tests_Boolean datatype/tests_Boolean.cpp)
target_link_libraries(tests_Boolean
        doctest
        rdf4cpp
        )
set_property(TARGET tests_Boolean PROPERTY CXX_STANDARD 20)
add_test(NAME tests_Boolean COMMAND tests_Boolean)


add_executable(tests_Decimal datatype/tests_Decimal.cpp)
target_link_libraries(tests_Decimal
        doctest
        rdf4cpp
        )
set_property(TARGET tests_Decimal PROPERTY CXX_STANDARD 20)
add_test(NAME tests_Decimal COMMAND tests_Decimal)

add_executable(tests_Float datatype/tests_Float.cpp)
target_link_libraries(tests_Float
        doctest
        rdf4cpp
        )
set_property(TARGET tests_Float PROPERTY CXX_STANDARD 20)
add_test(NAME tests_Float COMMAND tests_Float)

add_executable(tests_HexBinary datatype/tests_HexBinary.cpp)
target_link_libraries(tests_HexBinary
        doctest
        rdf4cpp
        )
set_property(TARGET tests_HexBinary PROPERTY CXX_STANDARD 20)
add_test(NAME tests_HexBinary COMMAND tests_HexBinary)

add_executable(tests_Base64Binary datatype/tests_Base64Binary.cpp)
target_link_libraries(tests_Base64Binary
        doctest
        rdf4cpp
        )
set_property(TARGET tests_Base64Binary PROPERTY CXX_STANDARD 20)
add_test(NAME tests_Base64Binary COMMAND tests_Base64Binary)

add_executable(tests_Double datatype/tests_Double.cpp)
target_link_libraries(tests_Double
        doctest
        rdf4cpp
        )
set_property(TARGET tests_Double PROPERTY CXX_STANDARD 20)
add_test(NAME tests_Double COMMAND tests_Double)

add_executable(tests_Integer datatype/tests_Integer.cpp)
target_link_libraries(tests_Integer
        doctest
        rdf4cpp
        )
set_property(TARGET tests_Integer PROPERTY CXX_STANDARD 20)
add_test(NAME tests_Integer COMMAND tests_Integer)

add_executable(tests_owl_Rational datatype/tests_owl_Rational.cpp)
target_link_libraries(tests_owl_Rational
        doctest
        rdf4cpp
        )
set_property(TARGET tests_owl_Rational PROPERTY CXX_STANDARD 20)
add_test(NAME tests_owl_Rational COMMAND tests_owl_Rational)

add_executable(tests_Byte datatype/tests_Byte.cpp)
target_link_libraries(tests_Byte
        doctest
        rdf4cpp
        )
set_property(TARGET tests_Byte PROPERTY CXX_STANDARD 20)
add_test(NAME tests_Byte COMMAND tests_Byte)

add_executable(tests_Short datatype/tests_Short.cpp)
target_link_libraries(tests_Short
        doctest
        rdf4cpp
        )
set_property(TARGET tests_Short PROPERTY CXX_STANDARD 20)
add_test(NAME tests_Short COMMAND tests_Short)

add_executable(tests_Int datatype/tests_Int.cpp)
target_link_libraries(tests_Int
        doctest
        rdf4cpp
        )
set_property(TARGET tests_Int PROPERTY CXX_STANDARD 20)
add_test(NAME tests_Int COMMAND tests_Int)

add_executable(tests_Long datatype/tests_Long.cpp)
target_link_libraries(tests_Long
        doctest
        rdf4cpp
        )
set_property(TARGET tests_Long PROPERTY CXX_STANDARD 20)
add_test(NAME tests_Long COMMAND tests_Long)

add_executable(tests_UnsignedByte datatype/tests_UnsignedByte.cpp)
target_link_libraries(tests_UnsignedByte
        doctest
        rdf4cpp
        )
set_property(TARGET tests_UnsignedByte PROPERTY CXX_STANDARD 20)
add_test(NAME tests_UnsignedByte COMMAND tests_UnsignedByte)

add_executable(tests_UnsignedShort datatype/tests_UnsignedShort.cpp)
target_link_libraries(tests_UnsignedShort
        doctest
        rdf4cpp
        )
set_property(TARGET tests_UnsignedShort PROPERTY CXX_STANDARD 20)
add_test(NAME tests_UnsignedShort COMMAND tests_UnsignedShort)

add_executable(tests_UnsignedInt datatype/tests_UnsignedInt.cpp)
target_link_libraries(tests_UnsignedInt
        doctest
        rdf4cpp
        )
set_property(TARGET tests_UnsignedInt PROPERTY CXX_STANDARD 20)
add_test(NAME tests_UnsignedInt COMMAND tests_UnsignedInt)

add_executable(tests_UnsignedLong datatype/tests_UnsignedLong.cpp)
target_link_libraries(tests_UnsignedLong
        doctest
        rdf4cpp
        )
set_property(TARGET tests_UnsignedLong PROPERTY CXX_STANDARD 20)
add_test(NAME tests_UnsignedLong COMMAND tests_UnsignedLong)

add_executable(tests_PositiveInteger datatype/tests_PositiveInteger.cpp)
target_link_libraries(tests_PositiveInteger
        doctest
        rdf4cpp
        )
set_property(TARGET tests_PositiveInteger PROPERTY CXX_STANDARD 20)
add_test(NAME tests_PositiveInteger COMMAND tests_PositiveInteger)

add_executable(tests_NonPositiveInteger datatype/tests_NonPositiveInteger.cpp)
target_link_libraries(tests_NonPositiveInteger
        doctest
        rdf4cpp
        )
set_property(TARGET tests_NonPositiveInteger PROPERTY CXX_STANDARD 20)
add_test(NAME tests_NonPositiveInteger COMMAND tests_NonPositiveInteger)

add_executable(tests_NegativeInteger datatype/tests_NegativeInteger.cpp)
target_link_libraries(tests_NegativeInteger
        doctest
        rdf4cpp
        )
set_property(TARGET tests_NegativeInteger PROPERTY CXX_STANDARD 20)
add_test(NAME tests_NegativeInteger COMMAND tests_NegativeInteger)

add_executable(tests_NonNegativeInteger datatype/tests_NonNegativeInteger.cpp)
target_link_libraries(tests_NonNegativeInteger
        doctest
        rdf4cpp
        )
set_property(TARGET tests_NonNegativeInteger PROPERTY CXX_STANDARD 20)
add_test(NAME tests_NonNegativeInteger COMMAND tests_NonNegativeInteger)

add_executable(tests_CowString util/tests_CowString.cpp)
target_link_libraries(tests_CowString
        doctest
        rdf4cpp
        )
set_property(TARGET tests_CowString PROPERTY CXX_STANDARD 20)
add_test(NAME tests_CowString COMMAND tests_CowString)

add_executable(tests_BlankNodeIdManagement util/tests_BlankNodeIdManagement.cpp)
target_link_libraries(tests_BlankNodeIdManagement
        doctest
        rdf4cpp
        )
set_property(TARGET tests_BlankNodeIdManagement PROPERTY CXX_STANDARD 20)
add_test(NAME tests_BlankNodeIdManagement COMMAND tests_BlankNodeIdManagement)

add_executable(tests_TriBool util/tests_TriBool.cpp)
target_link_libraries(tests_TriBool
        doctest
        rdf4cpp
        )
set_property(TARGET tests_TriBool PROPERTY CXX_STANDARD 20)
add_test(NAME tests_TriBool COMMAND tests_TriBool)

add_executable(tests_Regex util/tests_Regex.cpp)
target_link_libraries(tests_Regex
        doctest
        rdf4cpp
        )
set_property(TARGET tests_Regex PROPERTY CXX_STANDARD 20)
add_test(NAME tests_Regex COMMAND tests_Regex)

add_executable(tests_Inlining util/tests_Inlining.cpp)
target_link_libraries(tests_Inlining
        doctest
        rdf4cpp
        )
set_property(TARGET tests_Inlining PROPERTY CXX_STANDARD 20)
add_test(NAME tests_Inlining COMMAND tests_Inlining)

add_executable(tests_LangString datatype/tests_LangString.cpp)
target_link_libraries(tests_LangString
        doctest
        rdf4cpp
        )
set_property(TARGET tests_LangString PROPERTY CXX_STANDARD 20)
add_test(NAME tests_LangString COMMAND tests_LangString)

add_executable(tests_NumOpResults datatype/tests_NumOpResults.cpp)
target_link_libraries(tests_NumOpResults
        doctest
        rdf4cpp
        )
set_property(TARGET tests_NumOpResults PROPERTY CXX_STANDARD 20)
add_test(NAME tests_NumOpResults COMMAND tests_NumOpResults)

add_executable(tests_IStreamQuadIterator parser/tests_IStreamQuadIterator.cpp)
target_link_libraries(tests_IStreamQuadIterator
        doctest
        rdf4cpp
        )
set_property(TARGET tests_IStreamQuadIterator PROPERTY CXX_STANDARD 20)
add_test(NAME tests_IStreamQuadIterator COMMAND tests_IStreamQuadIterator)

add_executable(tests_Namespace_life_cycle namespaces/tests_Namespace_life_cycle.cpp)
target_link_libraries(tests_Namespace_life_cycle
        doctest
        rdf4cpp
        )
set_property(TARGET tests_Namespace_life_cycle PROPERTY CXX_STANDARD 20)
add_test(NAME tests_Namespace_life_cycle COMMAND tests_Namespace_life_cycle)

# copy files for testing to the binary folder
# file(COPY foldername DESTINATION ${CMAKE_CURRENT_BINARY_DIR})