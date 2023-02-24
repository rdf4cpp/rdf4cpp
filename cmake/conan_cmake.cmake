macro(install_packages_via_conan conanfile conan_options)
    if (FALSE) # TODO: we must wait until there is a cmake integration for conan again
        list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
        list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})


        #    if (NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
        #        message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
        #        file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
        #                "${CMAKE_BINARY_DIR}/conan.cmake"
        #                TLS_VERIFY ON)
        #    endif ()
        #    include(${CMAKE_BINARY_DIR}/conan.cmake)

        if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            SET(CONAN_COMPILER clang)
        elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            SET(CONAN_COMPILER gcc)
        elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
            SET(CONAN_COMPILER intel-cc)
        elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
            SET(CONAN_COMPILER msvc)
        endif ()

        # next 3 lines extract the major compiler version
        SET(CONAN_COMPILER_VERSION "${CMAKE_CXX_COMPILER_VERSION}")
        string(REPLACE "." ";" CONAN_COMPILER_VERSION "${CONAN_COMPILER_VERSION}")
        list(GET CONAN_COMPILER_VERSION 0 CONAN_COMPILER_VERSION)

        SET(CONAN_CPPSTD gnu23)

        if (BUILD_SHARED_LIBS)
            SET(CONAN_RUNTIME static)
        else ()
            SET(CONAN_RUNTIME dynamic)
        endif ()

        SET(CONAN_BUILD_TYPE "${CMAKE_BUILD_TYPE}")

        # there seems to be no way anymore to pass environment variables to conan install. Documentation is still Work in progress, see https://docs.conan.io/1/migrating_to_2.0/general.html#environment-variables
        set(ENV{CXX} "${CMAKE_CXX_COMPILER}")
        set(ENV{CC} "${CMAKE_C_COMPILER}")
        SET(BUILD_CMD conan install . -of ${CMAKE_BINARY_DIR} -s:h build_type=${CONAN_BUILD_TYPE} -s:b build_type=${CONAN_BUILD_TYPE} -s:h compiler=${CONAN_COMPILER} -s:b compiler=${CONAN_COMPILER} -s:h compiler.cppstd=${CONAN_CPPSTD} -s:b compiler.cppstd=${CONAN_CPPSTD} -s:h compiler.libcxx=libstdc++11 -s:b compiler.libcxx=libstdc++11 -s:h compiler.version=${CONAN_COMPILER_VERSION} -s:b compiler.version=${CONAN_COMPILER_VERSION} --build missing)
        message("${BUILD_CMD}")
        #    SET(BUILD_CMD "echo '${CXX} ${CC}'")
        execute_process(COMMAND ${BUILD_CMD}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                RESULT_VARIABLE CONAN_INSTALL_RESULT)
        if (NOT CONAN_INSTALL_RESULT EQUAL "0")
            message(FATAL_ERROR "${BUILD_CMD}\n${CONAN_INSTALL_RESULT}")
        endif ()

        #    conan_cmake_autodetect(settings)
        #    conan_check(VERSION 2 DETECT_QUIET)
        #    if (CONAN_CMD)
        #        conan_cmake_install(PATH_OR_REFERENCE ${conanfile} -s:b
        #                BUILD missing
        #                SETTINGS ${settings}
        #                OPTIONS "${conan_options}")
        #    else ()
        #        message(WARNING "No conan executable was found. Dependency retrieval via conan is disabled. System dependencies will be used if available.")
        #    endif ()
    endif ()
endmacro()