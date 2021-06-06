include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if (NOT DEFINED rdf4cpp_INSTALL_CMAKEDIR)
   set(rdf4cpp_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/rdf4cpp"
       CACHE STRING "Path to rdf4cpp CMake files")
endif ()

install(TARGETS rdf4cpp EXPORT rdf4cpp_Targets
        RUNTIME COMPONENT rdf4cpp_Runtime
        LIBRARY COMPONENT rdf4cpp_Runtime
        NAMELINK_COMPONENT rdf4cpp_Development
        ARCHIVE COMPONENT rdf4cpp_Development
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(DIRECTORY "${rdf4cpp_SOURCE_DIR}/src/"
        TYPE INCLUDE
        COMPONENT rdf4cpp_Development)

if (BUILD_SHARED_LIBS)
    set(type shared)
else ()
    set(type static)
endif ()

install(EXPORT rdf4cpp_Targets
        DESTINATION "${rdf4cpp_INSTALL_CMAKEDIR}"
        NAMESPACE rdf4cpp::
        FILE rdf4cpp-${type}-targets.cmake
        COMPONENT rdf4cpp_Development)

write_basic_package_version_file(
    rdf4cppConfigVersion.cmake
    COMPATIBILITY SameMajorVersion)

install(FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/rdf4cppConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/rdf4cppConfigVersion.cmake"
        DESTINATION "${rdf4cpp_INSTALL_CMAKEDIR}"
        COMPONENT rdf4cpp_Development)

# TODO: add additional CPack variables here
include(CPack)
