import os
import re

from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps

from conan import ConanFile

from conan.tools.files import load, copy, rmdir


class Recipe(ConanFile):
    author = "https://github.com/rdf4cpp"
    url = "https://github.com/rdf4cpp/rdf4cpp"
    description = "rdf4cpp aims to be a stable, modern RDF library for C++."
    topics = "rdf", "semantic-web", "sparql", "knowledge-graphs", "C++20"
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "with_test_deps": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "with_test_deps": False,
    }
    exports = "LICENSE",
    exports_sources = "src/*", "private/*", "CMakeLists.txt", "cmake/*"
    generators = ("CMakeDeps", "CMakeToolchain")

    def requirements(self):
        self.requires("boost/1.86.0", transitive_headers=True, libs=False)
        self.requires("expected-lite/0.6.3", transitive_headers=True)
        self.requires("re2/20221201")
        self.requires("openssl/3.0.8")
        self.requires("uni-algo/1.2.0")
        self.requires("highway/1.1.0")
        self.requires("dice-hash/0.4.5", transitive_headers=True)
        self.requires("dice-sparse-map/0.2.6", transitive_headers=True)
        self.requires("dice-template-library/1.5.1", transitive_headers=True)

        if self.options.with_test_deps:
            self.test_requires("doctest/2.4.11")
            self.test_requires("nanobench/4.3.11")

    def set_name(self):
        if not hasattr(self, 'name') or self.version is None:
            cmake_file = load(self, os.path.join(self.recipe_folder, "CMakeLists.txt"))
            self.name = re.search(r"project\(\s*([a-z0-9\-]+)\s+VERSION", cmake_file).group(1)

    def set_version(self):
        if not hasattr(self, 'version') or self.version is None:
            cmake_file = load(self, os.path.join(self.recipe_folder, "CMakeLists.txt"))
            self.version = re.search(r"project\([^)]*VERSION\s+(\d+\.\d+.\d+)[^)]*\)", cmake_file).group(1)

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)

    def generate(self):
        for _, dep in self.dependencies.items():
            if dep.package_folder is None:
                continue
            copy(self, "*LICENSE*", src=dep.package_folder, dst=os.path.join(self.recipe_folder, "licenses", dep.ref.name), ignore_case=True)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
        rmdir(self, os.path.join(self.package_folder, "cmake"))
        rmdir(self, os.path.join(self.package_folder, "share"))
        copy(self, "LICENSE", src=self.recipe_folder, dst=os.path.join(self.package_folder, "licenses"))
        copy(self, "*", os.path.join(self.recipe_folder, "licenses"), dst=os.path.join(self.package_folder, "licenses"))
        self.output.info(copy(self, "COPYING", src=os.path.join(self.build_folder, "serd"), dst=os.path.join(self.package_folder, "licenses", "serd")))

    def package_info(self):
        self.cpp_info.libs = ["rdf4cpp"]
        self.cpp_info.set_property("cmake_find_mode", "both")
        self.cpp_info.set_property("cmake_target_name", "rdf4cpp::rdf4cpp")
        self.cpp_info.set_property("cmake_file_name", "rdf4cpp")
