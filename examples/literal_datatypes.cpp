#include <rdf4cpp/rdf.hpp>
#include <cassert>

using namespace rdf4cpp::rdf;

void value_construction() {
    // runtime construction
    [[maybe_unused]] Literal float_1{"1.1", IRI{"http://www.w3.org/2001/XMLSchema#float"}};

    // using compile time knowledge
    [[maybe_unused]] Literal double_1 = Literal::make<datatypes::xsd::Double>(1.2);

    // simplification: using user-defined-literal function
    [[maybe_unused]] Literal int_1 = 1_xsd_int;
}

void lexical_access() {
    Literal double_1 = 1.8_xsd_double;
    assert(double_1.lexical_form() == "1.8E0");
    assert(std::string{double_1} == R"#("1.8E0"^^<http://www.w3.org/2001/XMLSchema#double>)#");

    Literal lang_string{"Hello", "en-US"};
    assert(lang_string.lexical_form() == "Hello");
    assert(lang_string.language_tag() == "en-US");
    assert(lang_string.lang_matches("*"));
    assert(lang_string.lang_matches("en"));
    assert(std::string{lang_string} == R"#("Hello"@en-US)#");
}

void direct_value_access() {
    Literal lit = 1.1_xsd_float;

    std::any rt_value = lit.value();
    assert(any_cast<float>(rt_value) == 1.1f);

    float ct_value = lit.value<datatypes::xsd::Float>();
    assert(ct_value == 1.1f);

    try {
        // invalid access
        lit.value<datatypes::xsd::Integer>();
        assert(false);
    } catch (std::runtime_error const &) {
    }
}

void value_transformations() {
    Literal lit = 42.0_xsd_float;
    assert(lit.value<datatypes::xsd::Float>() == 42.f);

    lit = lit * -1.5_xsd_float;
    assert(lit.value<datatypes::xsd::Float>() == -63.f);

    lit = lit.abs();
    assert(lit.value<datatypes::xsd::Float>() == 63.f);

    lit = lit.cast<datatypes::xsd::String>();
    assert(lit.value<datatypes::xsd::String>() == "63");

    lit = lit.ebv_as_literal();
    assert(lit.value<datatypes::xsd::Boolean>() == true);
}

void comparisons() {
    Literal lit1 = 42.0_xsd_float;
    Literal lit2 = 89_xsd_int;
    Literal lit3 = 42_xsd_integer;

    assert(lit1 < lit2);
    assert(lit1 == lit3);
    assert(lit2 > lit3);
    assert(lit1.compare_with_extensions(lit3) == std::partial_ordering::less);
}

int main() {
    value_construction();
    lexical_access();
    direct_value_access();
    value_transformations();
    comparisons();
}
