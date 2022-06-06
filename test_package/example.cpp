#include <rdf4cpp/rdf.hpp>

#include <iostream>

int main() {
    std::cout << rdf4cpp::rdf::Literal("Using the conan package works.") << std::endl;
}