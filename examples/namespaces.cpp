#include <rdf4cpp.hpp>

#include <iostream>

int main() {
    using namespace rdf4cpp;

    Namespace ex("http://example.com/");

    IRI ex_A = ex + "A";
    std::cout << ex_A << std::endl;
    assert(ex_A.identifier() == "http://example.com/A");

    ex.clear();
    ex_A = ex + "A";
    std::cout << ex_A << std::endl;
    assert(ex_A.identifier() == "http://example.com/A");

    auto rdf = namespaces::RDF();
    auto rdf_Property = rdf + "Property";
    std::cout << rdf_Property << std::endl;
    assert(rdf_Property.identifier() == "http://www.w3.org/1999/02/22-rdf-syntax-ns#Property");

    auto rdf_15 = rdf + "_15";
    std::cout << rdf_15 << std::endl;
    assert(rdf_15.identifier() == "http://www.w3.org/1999/02/22-rdf-syntax-ns#_15");
    try {
        [[maybe_unused]] auto random = rdf + "random";
    } catch (std::runtime_error const &ex) {
        std::cerr << ex.what() << std::endl;
    }
}