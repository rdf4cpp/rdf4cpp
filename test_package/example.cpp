#include <rdf4cpp/rdf.hpp>

#include <iostream>

int main() {
    using namespace rdf4cpp::rdf;

    [[maybe_unused]] Literal datatype_test_lit{"123.0", IRI{"http://www.w3.org/2001/XMLSchema#double"}}; // segfault if registering of datatypes does not work
    std::cout << Literal{"Using the conan package works."} << std::endl;
}