#include <rdf4cpp.hpp>
#include <rdf4cpp/parser/RDFFileParser.hpp>

int main() {
    // read file
    for (const auto &v : rdf4cpp::parser::RDFFileParser{"./RDFFileParser_simple.ttl"}) {
        if (v.has_value())  // check if parser returns a successful read value
            std::cout << v.value() << "\n";
        else  // or an error
            std::cerr << v.error() << "\n";
    }
    return 0;
}