cmake_minimum_required(VERSION 3.17)
project(FetchContentTest CXX)

# find out the current tag of the parent repos in the folder above
execute_process(COMMAND git rev-parse HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../
        OUTPUT_VARIABLE parent_git_tag
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
# fetch the repo from the local copy
# beware: only submitted changes are checked out!
include(FetchContent)
FetchContent_Declare(
        rdf4cpp
        GIT_REPOSITORY "${CMAKE_CURRENT_SOURCE_DIR}/../"
        GIT_TAG ${parent_git_tag}
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(rdf4cpp)
add_executable(example src/example.cpp)
# With FetchContent_Declare we don't need find_package
target_link_libraries(example rdf4cpp::rdf4cpp)
set_property(TARGET example PROPERTY CXX_STANDARD 20)