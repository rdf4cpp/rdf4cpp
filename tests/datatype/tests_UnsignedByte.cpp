#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("Datatype UnsignedByte") {

    constexpr auto correct_type_iri_cstr = "http://www.w3.org/2001/XMLSchema#unsignedByte";

    CHECK(std::string(datatypes::xsd::UnsignedByte::identifier) == correct_type_iri_cstr);

    auto type_iri = IRI(datatypes::xsd::UnsignedByte::identifier);
    CHECK(type_iri.identifier() == correct_type_iri_cstr);

    using type = datatypes::xsd::UnsignedByte::cpp_type;

    CHECK(std::is_same_v<type, uint8_t>);

    uint8_t value = 1;
    auto lit1 = Literal::make<datatypes::xsd::UnsignedByte>(value);
    CHECK(lit1.value<datatypes::xsd::UnsignedByte>() == value);
    CHECK(lit1.lexical_form() == std::to_string(value));

    value = 0;
    auto lit2 = Literal::make<datatypes::xsd::UnsignedByte>(value);
    CHECK(lit2.value<datatypes::xsd::UnsignedByte>() == value);
    CHECK(lit2.lexical_form() == std::to_string(value));

    value = 255;
    auto lit3 = Literal::make<datatypes::xsd::UnsignedByte>(value);
    CHECK(lit3.value<datatypes::xsd::UnsignedByte>() == value);
    CHECK(lit3.lexical_form() == std::to_string(value));

    value = 1;
    auto lit4 = Literal::make<datatypes::xsd::UnsignedByte>(value);
    CHECK(lit4.value<datatypes::xsd::UnsignedByte>() == value);
    CHECK(lit4.lexical_form() == std::to_string(value));

    CHECK(lit1 != lit2);
    CHECK(lit2 != lit3);
    CHECK(lit1 == lit4);

    // suppress warnings regarding attribute ‘nodiscard’
    Literal no_discard_dummy;

    CHECK_THROWS(no_discard_dummy = Literal("256", type_iri));

    CHECK_THROWS(no_discard_dummy = Literal("-1", type_iri));

    CHECK_THROWS(no_discard_dummy = Literal("a23dg.59566", type_iri));

    CHECK_THROWS(no_discard_dummy = Literal("-0.00001", type_iri));

    CHECK_THROWS(no_discard_dummy = Literal("qwerty", type_iri));
}
