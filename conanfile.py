import os
import re

from conan.tools.cmake import CMake, cmake_layout, CMakeToolchain, CMakeDeps

from conan import ConanFile

from conan.tools.files import load, rmdir, copy


class Recipe(ConanFile):
    name = "rdf4cpp"
    version = "0.0.11"

    author = "https://github.com/rdf4cpp"
    url = "https://github.com/rdf4cpp/rdf4cpp"
    description = "rdf4cpp aims to be a stable, modern RDF library for C++."
    topics = ("rdf", "semantic-web", "sparql", "knowledge-graphs", "C++20")

    # Binary configuration
    package_type = "library"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    exports = "LICENSE",
    exports_sources = "src/*", "private/*", "CMakeLists.txt", "cmake/*"

    def requirements(self):
        transitive = {"headers": True, "libs": True, "transitive_headers": True, "transitive_libs": True}
        transitive_header_only = {"headers": True, "transitive_headers": True}
        header_only_non_transitive = {"headers": True, "libs": False, "build": False, "run": False,
                                      "transitive_libs": False, "transitive_headers": False}

        self.requires("expected-lite/0.6.2", **transitive_header_only)
        self.requires("boost/1.81.0", **transitive)
        self.requires("re2/20230201", **transitive)

        self.requires("fmt/9.1.0", **header_only_non_transitive)
        self.requires("utfcpp/3.2.3", **header_only_non_transitive)

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")
        self.options["fmt"].header_only = True

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    _cmake = None

    def _configure_cmake(self):
        if not self._cmake:
            self._cmake = CMake(self)
            self._cmake.configure(variables={"USE_CONAN": False, "BUILD_TESTING": False})
        return self._cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()
        rmdir(self, os.path.join(self.package_folder, "share"))
        copy(self, "LICENSE", src=self.recipe_folder, dst="licenses")
        copy(self, os.path.join("serd", "COPYING"), src=self.build_folder, dst="licenses")

    def package_info(self):
        self.cpp_info.libs = ["rdf4cpp"]
