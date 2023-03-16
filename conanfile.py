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
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    exports = "LICENSE",
    exports_sources = "src/*", "private/*", "CMakeLists.txt", "cmake/*"
    requires = (("fmt/9.0.0", "private"),  # format must only be used within cpp files
                "expected-lite/0.6.2",
                "boost/1.79.0",
                "re2/20221201",
                ("utfcpp/3.2.3", "private"),
                "openssl/3.0.8",
                "uni-algo/0.7.1@rdf4cpp/temporary")

    generators = ("CMakeDeps", "CMakeToolchain")

    def set_version(self):
        if not hasattr(self, 'version') or self.version is None:
            cmake_file = load(self, os.path.join(self.recipe_folder, "CMakeLists.txt"))
            self.version = re.search(r"project\([^)]*VERSION\s+(\d+\.\d+.\d+)[^)]*\)", cmake_file).group(1)

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC
        self.options["fmt"].header_only = True

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
