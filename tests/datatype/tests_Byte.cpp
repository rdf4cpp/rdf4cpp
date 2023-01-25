#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("Datatype Byte") {

    constexpr auto correct_type_iri_cstr = "http://www.w3.org/2001/XMLSchema#byte";

    CHECK(std::string(datatypes::xsd::Byte::identifier) == correct_type_iri_cstr);

    auto type_iri = IRI(datatypes::xsd::Byte::identifier);
    CHECK(type_iri.identifier() == correct_type_iri_cstr);

    using type = datatypes::xsd::Byte::cpp_type;

    CHECK(std::is_same_v<type, int8_t>);

    auto value = 1;
    auto lit1 = Literal::make_typed<datatypes::xsd::Byte>(value);
    CHECK(lit1.value<datatypes::xsd::Byte>() == value);
    CHECK(lit1.lexical_form() == std::to_string(value));

    value = -128;
    auto lit2 = Literal::make_typed<datatypes::xsd::Byte>(value);
    CHECK(lit2.value<datatypes::xsd::Byte>() == value);
    CHECK(lit2.lexical_form() == std::to_string(value));

    value = 127;
    auto lit3 = Literal::make_typed<datatypes::xsd::Byte>(value);
    CHECK(lit3.value<datatypes::xsd::Byte>() == value);
    CHECK(lit3.lexical_form() == std::to_string(value));

    value = 1;
    auto lit4 = Literal::make_typed(std::to_string(value), type_iri);
    CHECK(lit4.value<datatypes::xsd::Byte>() == value);
    CHECK(lit4.lexical_form() == std::to_string(value));

    value = 127;
    auto lit5 = Literal::make_typed(std::to_string(value), type_iri);
    CHECK(lit5.value<datatypes::xsd::Byte>() == value);
    CHECK(lit5.lexical_form() == std::to_string(value));

    CHECK(lit1 != lit2);
    CHECK(lit2 != lit3);
    CHECK(lit1 == lit4);
    CHECK(lit3 == lit5);

    // suppress warnings regarding attribute ‘nodiscard’
    Literal no_discard_dummy;

    CHECK_THROWS(no_discard_dummy = Literal::make_typed("139", type_iri));

    CHECK_THROWS(no_discard_dummy = Literal::make_typed("-130", type_iri));

    CHECK_THROWS(no_discard_dummy = Literal::make_typed("0.00001", type_iri));

    CHECK_THROWS(no_discard_dummy = Literal::make_typed("qwerty", type_iri));

    CHECK_THROWS(no_discard_dummy = Literal::make_typed("a23dg.59566", type_iri));
}
