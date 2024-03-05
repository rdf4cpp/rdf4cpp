#include "rdf4cpp.hpp"

int main() {
    using namespace rdf4cpp;
    Literal a = Literal::make_typed_from_value<datatypes::xsd::Double>(1.2);
    Literal b = Literal::make_typed("1.1", IRI{"http://www.w3.org/2001/XMLSchema#float"});
    Literal r = a + b;
    std::cout << r.lexical_form();
    return 0;
}
