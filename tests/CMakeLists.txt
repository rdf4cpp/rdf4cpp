## C++ language configuration boilerplate

include(FetchContent)

FetchContent_Declare(
        DocTest
        GIT_REPOSITORY "https://github.com/onqtam/doctest"
        GIT_TAG  "2.4.6"
)
FetchContent_MakeAvailable(DocTest)

# add the executable for all tests
add_executable(tests_hello_test hello_test/main.cpp)
target_link_libraries(tests_hello_test
        doctest
        rdf4cpp
        )
set_property(TARGET tests_hello_test PROPERTY CXX_STANDARD 20)
add_test(NAME tests_hello_test COMMAND tests_hello_test)


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


# include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
# doctest_discover_tests(tests_hello_test)

# copy files for testing to the binary folder
# file(COPY foldername DESTINATION ${CMAKE_CURRENT_BINARY_DIR})