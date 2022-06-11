import os
import re

from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout, CMakeDeps, CMakeToolchain
from conans.util.files import rmdir, load


class RDF4CPPConan(ConanFile):
    name = "rdf4cpp"
    version = None

    author = "https://github.com/rdf4cpp"
    url = "https://github.com/rdf4cpp/rdf4cpp"
    description = "rdf4cpp aims to be a stable, modern RDF library for C++."
    topics = ("rdf", "semantic web", "sparql", "knowledge graphs", "C++20")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    exports_sources = "src/*", "CMakeLists.txt", "cmake/*"

    generators = ("CMakeDeps", "CMakeToolchain")


    def set_version(self):
        if not hasattr(self, 'version') or self.version is None:
            cmake_file = load(os.path.join(self.recipe_folder, "CMakeLists.txt"))
            self.version = re.search(r"project\([^)]*VERSION\s+(\d+\.\d+.\d+)[^)]*\)", cmake_file).group(1)

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    _cmake = None

    def _configure_cmake(self):
        if self._cmake:
            return self._cmake
        self._cmake = CMake(self)
        self._cmake.configure()

        return self._cmake

    def layout(self):
        cmake_layout(self)

    def build(self):
        self._configure_cmake().build()

    def package(self):
        self._configure_cmake().install()
        rmdir(os.path.join(self.package_folder, "cmake"))

    def package_info(self):
        self.cpp_info.includedirs = ["include"]
        self.cpp_info.libdirs = ["lib"]
        self.cpp_info.libs = ["rdf4cpp"]
