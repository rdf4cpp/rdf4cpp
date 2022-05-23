#include <rdf4cpp/rdf.hpp>


#include <iostream>

int main() {
    using namespace rdf4cpp::rdf;

    Literal float_1_1("1.1", IRI{"http://www.w3.org/2001/XMLSchema#float"});

    std::cout << float_1_1 << std::endl;
    std::any any_float_ = float_1_1.value();
    std::cout << any_cast<float>(any_float_) << std::endl;
    auto float_ = float_1_1.value<float, datatypes::xsd_float>();  // we know the type at compile time
    [[maybe_unused]] auto int_ = float_1_1.value<int, datatypes::xsd_int>();
    std::cout << float_ << std::endl;

    // update value
    float_ *= any_cast<datatypes::xsd::Float>(any_float_) * 3;
    // datatypes::xsd::Float is an alias for the built-in type float
    std::cout << float_ << std::endl;
    // make a new literal with new value
    Literal updated_float = Literal::make<float, datatypes::xsd_float>(float_);
    std::cout << updated_float << std::endl;
}