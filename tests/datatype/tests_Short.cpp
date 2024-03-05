#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include "rdf4cpp.hpp"

using namespace rdf4cpp::rdf;

TEST_CASE("Datatype Short") {

    constexpr auto correct_type_iri_cstr = "http://www.w3.org/2001/XMLSchema#short";

    CHECK(std::string(datatypes::xsd::Short::identifier) == correct_type_iri_cstr);

    auto type_iri = IRI(datatypes::xsd::Short::identifier);
    CHECK(type_iri.identifier() == correct_type_iri_cstr);

    using type = datatypes::xsd::Short::cpp_type;

    CHECK(std::is_same_v<type, int16_t>);

    int16_t value = 1;
    auto lit1 = Literal::make_typed_from_value<datatypes::xsd::Short>(value);
    CHECK(lit1.value<datatypes::xsd::Short>() == value);
    CHECK(lit1.lexical_form() == std::to_string(value));

    value = -32768;
    auto lit2 = Literal::make_typed_from_value<datatypes::xsd::Short>(value);
    CHECK(lit2.value<datatypes::xsd::Short>() == value);
    CHECK(lit2.lexical_form() == std::to_string(value));

    value = 32767;
    auto lit3 = Literal::make_typed_from_value<datatypes::xsd::Short>(value);
    CHECK(lit3.value<datatypes::xsd::Short>() == value);
    CHECK(lit3.lexical_form() == std::to_string(value));

    value = 1;
    auto lit4 = Literal::make_typed(std::to_string(value), type_iri);
    CHECK(lit4.value<datatypes::xsd::Short>() == value);

    value = -32768;
    auto lit5 = Literal::make_typed(std::to_string(value), type_iri);
    CHECK(lit5.value<datatypes::xsd::Short>() == value);

    value = std::numeric_limits<int16_t>::min();
    auto lit6 = Literal::make_typed_from_value<datatypes::xsd::Int>(value);
    CHECK(lit6.value<datatypes::xsd::Int>() == value);
    CHECK(lit6.lexical_form() == std::to_string(value));

    value = std::numeric_limits<int16_t>::max();
    auto lit7 = Literal::make_typed_from_value<datatypes::xsd::Int>(value);
    CHECK(lit7.value<datatypes::xsd::Int>() == value);
    CHECK(lit7.lexical_form() == std::to_string(value));

    value = 0;
    auto lit8 = Literal::make_typed_from_value<datatypes::xsd::Int>(value);
    CHECK(lit8.value<datatypes::xsd::Int>() == value);
    CHECK(lit8.lexical_form() == std::to_string(value));

    CHECK(lit1 != lit2);
    CHECK(lit2 != lit3);
    CHECK(lit1 == lit4);
    CHECK(lit2 == lit5);
    CHECK(lit2 == lit6);
    CHECK(lit3 == lit7);
    CHECK(lit5 == lit6);

    // suppress warnings regarding attribute ‘nodiscard’
    Literal no_discard_dummy;

    CHECK_THROWS(no_discard_dummy = Literal::make_typed("32768", type_iri));

    CHECK_THROWS(no_discard_dummy = Literal::make_typed("-32769", type_iri));

    CHECK_THROWS(no_discard_dummy = Literal::make_typed("qwerty", type_iri));

    CHECK_THROWS(no_discard_dummy = Literal::make_typed("-32768.0001", type_iri));

    CHECK_THROWS(no_discard_dummy = Literal::make_typed("a23dg.59566", type_iri));
}
