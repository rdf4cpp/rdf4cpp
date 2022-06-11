macro(boilerplate_init)
    ## enforce standard compliance
    set(CMAKE_CXX_STANDARD_REQUIRED True)
    set(CMAKE_CXX_EXTENSIONS OFF)

    ## C++ compiler flags
    if (MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall")
    else ()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
    endif ()

    ## C++ language visibility configuration
    if (NOT DEFINED CMAKE_CXX_VISIBILITY_PRESET AND
            NOT DEFINED CMAKE_VISIBILITY_INLINES_HIDDEN)
        set(CMAKE_CXX_VISIBILITY_PRESET default)
        set(CMAKE_VISIBILITY_INLINES_HIDDEN NO)
    endif ()

    string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" IS_TOP_LEVEL)
endmacro()
