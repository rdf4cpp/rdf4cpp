#include <rdf4cpp/rdf.hpp>

#include <iostream>

int main() {
    using namespace rdf4cpp::rdf;

    Literal datatype_test_lit{"123.0", IRI{"http://www.w3.org/2001/XMLSchema#double"}}; // segfault if registering of datatypes does not work
    std::cout << datatype_test_lit << std::endl;
    std::cout << Literal{"Using FetchContent works."} << std::endl;
}