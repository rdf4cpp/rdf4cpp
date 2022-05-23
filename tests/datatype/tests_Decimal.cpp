#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("Datatype Decimal") {

    constexpr auto correct_type_iri_cstr = "http://www.w3.org/2001/XMLSchema#decimal";

    CHECK(std::string(datatypes::xsd::Decimal::identifier) == correct_type_iri_cstr);

    auto type_iri = IRI(datatypes::xsd::Decimal::identifier);
    CHECK(type_iri.identifier() == correct_type_iri_cstr);

    using type = datatypes::xsd::Decimal::cpp_type;

    CHECK(std::is_same_v<type, double>);

    double value = 1.00;
    auto lit1 = Literal::make<datatypes::xsd::Decimal>(value);
    CHECK(lit1.value<datatypes::xsd::Decimal>() == value);
    CHECK(lit1.lexical_form() == std::to_string(value));

    value = 64582165456988.235046;
    auto lit2 = Literal::make<datatypes::xsd::Decimal>(value);
    CHECK(lit2.value<datatypes::xsd::Decimal>() == value);
    CHECK(lit2.lexical_form() == std::to_string(value));

    value = -64524654389.2345650;
    auto lit3 = Literal::make<datatypes::xsd::Decimal>(value);
    CHECK(lit3.value<datatypes::xsd::Decimal>() == value);
    CHECK(lit3.lexical_form() == std::to_string(value));

    value = 1;
    auto lit4 = Literal{std::to_string(value), type_iri};
    CHECK(lit4.value<datatypes::xsd::Decimal>() == value);

    value = 64582165456988.235046;
    auto lit5 = Literal{std::to_string(value), type_iri};
    CHECK(lit5.value<datatypes::xsd::Decimal>() == value);

    CHECK(lit1 != lit2);
    CHECK(lit2 != lit3);
    CHECK(lit1 == lit4);
    CHECK(lit2 == lit5);

    // suppress warnings regarding attribute ‘nodiscard’
    std::any no_discard_dummy = false;

    auto lit6 = Literal{"NAN", type_iri};
    CHECK_THROWS_WITH_AS(no_discard_dummy = lit6.value(), "XSD Parsing Error", std::runtime_error);

    auto lit7 = Literal{"INF", type_iri};
    CHECK_THROWS_WITH_AS(no_discard_dummy = lit7.value(), "XSD Parsing Error", std::runtime_error);

    value = INFINITY;
    auto lit8 = Literal{std::to_string(value), type_iri};
    CHECK_THROWS_WITH_AS(no_discard_dummy = lit8.value(), "XSD Parsing Error", std::runtime_error);

    value = NAN;
    auto lit9 = Literal{std::to_string(value), type_iri};
    CHECK_THROWS_WITH_AS(no_discard_dummy = lit9.value(), "XSD Parsing Error", std::runtime_error);

    auto lit10 = Literal{"454sdsd", type_iri};
    CHECK_THROWS_WITH_AS(no_discard_dummy = lit10.value(), "XSD Parsing Error", std::runtime_error);
}