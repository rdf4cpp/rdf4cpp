## C++ language configuration boilerplate

include(FetchContent)

FetchContent_Declare(
        DocTest
        GIT_REPOSITORY "https://github.com/onqtam/doctest"
        GIT_TAG  "2.4.6"
)
FetchContent_MakeAvailable(DocTest)

# add the executable for all tests
add_executable(tests_hello_test hello_test/main.cpp query/tests_Variable.cpp query/tests_TriplePattern.cpp query/tests_QuadPattern.cpp nodes/tests_Literal.cpp)
#[[add_executable(tests_hello_test hello_test/main.cpp)
add_executable(tests_Variable query/tests_Variable.cpp)
add_executable(tests_TriplePattern query/tests_TriplePattern.cpp)
add_executable(tests_QuadPattern query/tests_QuadPattern.cpp)
add_executable(tests_Literal nodes/tests_Literal.cpp)]]
target_link_libraries(tests_hello_test
        doctest
        rdf4cpp
        )

set_property(TARGET tests_hello_test PROPERTY CXX_STANDARD 20)

# include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
# doctest_discover_tests(tests_hello_test)

# copy files for testing to the binary folder
# file(COPY foldername DESTINATION ${CMAKE_CURRENT_BINARY_DIR})