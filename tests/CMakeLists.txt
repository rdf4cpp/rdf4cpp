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


add_executable(tests_Node_comparison nodes/tests_Node_comparison.cpp)
target_link_libraries(tests_Node_comparison
        doctest
        rdf4cpp
        )
set_property(TARGET tests_Node_comparison PROPERTY CXX_STANDARD 20)
add_test(NAME tests_Node_comparison COMMAND tests_Node_comparison)

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

add_executable(tests_Integer datatype/tests_Integer.cpp)
target_link_libraries(tests_Integer
        doctest
        rdf4cpp
        )
set_property(TARGET tests_Integer PROPERTY CXX_STANDARD 20)
add_test(NAME tests_Integer COMMAND tests_Integer)

#Recurring and partial date datatypes
add_executable(tests_GDay datatype/tests_GDay.cpp)
target_link_libraries(tests_GDay
        doctest
        rdf4cpp
        )
set_property(TARGET tests_GDay PROPERTY CXX_STANDARD 20)
add_test(NAME tests_GDay COMMAND tests_GDay)

add_executable(tests_GMonth datatype/tests_GMonth.cpp)
target_link_libraries(tests_GMonth
        doctest
        rdf4cpp
        )
set_property(TARGET tests_GMonth PROPERTY CXX_STANDARD 20)
add_test(NAME tests_GMonth COMMAND tests_GDay)

add_executable(tests_GYear datatype/tests_GYear.cpp)
target_link_libraries(tests_GYear
        doctest
        rdf4cpp
        )
set_property(TARGET tests_GYear PROPERTY CXX_STANDARD 20)
add_test(NAME tests_GYear COMMAND tests_GYear)

add_executable(tests_GYearMonth datatype/tests_GYearMonth.cpp)
target_link_libraries(tests_GYearMonth
        doctest
        rdf4cpp
        )
set_property(TARGET tests_GYearMonth PROPERTY CXX_STANDARD 20)
add_test(NAME tests_GYearMonth COMMAND tests_GYearMonth)

add_executable(tests_GMonthDay datatype/tests_GMonthDay.cpp)
target_link_libraries(tests_GMonthDay
        doctest
        rdf4cpp
        )
set_property(TARGET tests_GMonthDay PROPERTY CXX_STANDARD 20)
add_test(NAME tests_GMonthDay COMMAND tests_GMonthDay)

add_executable(tests_Duration datatype/tests_Duration.cpp)
target_link_libraries(tests_Duration
        doctest
        rdf4cpp
        )
set_property(TARGET tests_Duration PROPERTY CXX_STANDARD 20)
add_test(NAME tests_Duration COMMAND tests_Duration)

add_executable(tests_YearMonthDuration datatype/tests_YearMonthDuration.cpp)
target_link_libraries(tests_YearMonthDuration
        doctest
        rdf4cpp
        )
set_property(TARGET tests_YearMonthDuration PROPERTY CXX_STANDARD 20)
add_test(NAME tests_YearMonthDuration COMMAND tests_YearMonthDuration)

add_executable(tests_DayTimeDuration datatype/tests_DayTimeDuration.cpp)
target_link_libraries(tests_DayTimeDuration
        doctest
        rdf4cpp
        )
set_property(TARGET tests_DayTimeDuration PROPERTY CXX_STANDARD 20)
add_test(NAME tests_DayTimeDuration COMMAND tests_DayTimeDuration)

# copy files for testing to the binary folder
# file(COPY foldername DESTINATION ${CMAKE_CURRENT_BINARY_DIR})