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
add_test(test_test tests_hello_test COMMAND tests_hello_test)

set_property(TARGET tests_hello_test PROPERTY CXX_STANDARD 20)

# include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
# doctest_discover_tests(tests_hello_test)

# copy files for testing to the binary folder
# file(COPY foldername DESTINATION ${CMAKE_CURRENT_BINARY_DIR})