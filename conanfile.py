import os
import re

from conans import ConanFile, CMake
from conans import tools


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
    exports_sources = "src/*", "CMakeLists.txt", "cmake/*"
    requires = ("serd/0.30.12",)

    generators = ("CMakeDeps", "CMakeToolchain")

    def set_version(self):
        if not hasattr(self, 'version') or self.version is None:
            cmake_file = tools.load(os.path.join(self.recipe_folder, "CMakeLists.txt"))
            self.version = re.search(r"project\([^)]*VERSION\s+(\d+\.\d+.\d+)[^)]*\)", cmake_file).group(1)

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    _cmake = None

    def _configure_cmake(self):
        if self._cmake:
            return self._cmake
        self._cmake = CMake(self)
        self._cmake.definitions["USE_CONAN"] = False
        self._cmake.configure()

        return self._cmake

    def build(self):
        self._configure_cmake().build()

    def package(self):
        self._configure_cmake().install()
        tools.rmdir(os.path.join(self.package_folder, "cmake"))
        tools.rmdir(os.path.join(self.package_folder, "share"))
        self.copy("LICENSE", src=self.folders.base_source, dst="licenses")

    def package_info(self):
        self.cpp_info.libs = ["rdf4cpp"]
