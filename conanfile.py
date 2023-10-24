import os
import re

from conan.tools.cmake import CMake

from conan import ConanFile

from conan.tools.files import load, copy, rmdir


class Recipe(ConanFile):
    name = "rdf4cpp"
    version = None

    author = "https://github.com/rdf4cpp"
    url = "https://github.com/rdf4cpp/rdf4cpp"
    description = "rdf4cpp aims to be a stable, modern RDF library for C++."
    topics = ("rdf", "semantic-web", "sparql", "knowledge-graphs", "C++20")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False], "with_test_deps": [True, False]}
    default_options = {"shared": False, "fPIC": True, "with_test_deps": False}
    exports = "LICENSE",
    exports_sources = "src/*", "private/*", "CMakeLists.txt", "cmake/*"

    generators = ("CMakeDeps", "CMakeToolchain")

    def requirements(self):
        self.requires("boost/1.81.0")
        self.requires("expected-lite/0.6.2")
        self.requires("re2/20221201")
        self.requires("openssl/3.0.8")
        self.requires("uni-algo/0.7.1")
        self.requires("dice-hash/0.4.3")
        self.requires("dice-sparse-map/0.2.4")

        if self.options.with_test_deps:
            self.requires("doctest/2.4.11")
            self.requires("nanobench/4.3.11")
            self.requires("libcurl/7.85.0")

    def set_version(self):
        if not hasattr(self, 'version') or self.version is None:
            cmake_file = load(self, os.path.join(self.recipe_folder, "CMakeLists.txt"))
            self.version = re.search(r"project\([^)]*VERSION\s+(\d+\.\d+.\d+)[^)]*\)", cmake_file).group(1)

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    _cmake = None

    def _configure_cmake(self):
        if self._cmake:
            return self._cmake
        self._cmake = CMake(self)
        self._cmake.configure(variables={"USE_CONAN": False})

        return self._cmake

    def build(self):
        self._configure_cmake().build()

    def package(self):
        self._configure_cmake().install()
        rmdir(self, os.path.join(self.package_folder, "cmake"))
        rmdir(self, os.path.join(self.package_folder, "share"))
        copy(self, "LICENSE", src=self.recipe_folder, dst="licenses")
        copy(self, os.path.join("serd", "COPYING"), src=self.build_folder, dst="licenses")

    def package_info(self):
        self.cpp_info.libs = ["rdf4cpp"]
