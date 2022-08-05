#include <rdf4cpp/rdf.hpp>

#include <iostream>

int main() {
    using namespace rdf4cpp::rdf;

    Namespace ex("http://example.com/");

    auto ex_A = ex + "A";
    std::cout << ex_A << std::endl;
    ex.clear();
    ex_A = ex + "A";

    auto rdf = namespaces::RDF();
    auto rdf_Property = rdf + "Property";
    std::cout << rdf_Property << std::endl;

    auto rdf_15 = rdf + "_15";
    std::cout << rdf_15 << std::endl;
    try {
        auto random = rdf + "random";
    } catch (std::runtime_error const &ex) {
        std::cerr << ex.what() << std::endl;
    }

    auto rdfs = namespaces::RDFS();
}