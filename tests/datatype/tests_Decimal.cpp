#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf::datatypes;

TEST_CASE("Datatype Decimal") {

    auto iri = rdf4cpp::rdf::IRI(RegisteredDatatype<xsd::Decimal, xsd_decimal>::datatype_iri());

    auto iri_str = rdf4cpp::rdf::IRI("http://www.w3.org/2001/XMLSchema#decimal");

    CHECK(iri == iri_str);

    double value = 1.00;
    auto lit1 = rdf4cpp::rdf::Literal::make<xsd::Decimal, xsd_decimal>(value);
    CHECK(lit1.value<xsd::Decimal, xsd_decimal>() == value);
    CHECK(lit1.lexical_form() == std::to_string(value));

    value = 64582165456988.235046;
    auto lit2 = rdf4cpp::rdf::Literal::make<xsd::Decimal, xsd_decimal>(value);
    CHECK(lit2.value<xsd::Decimal, xsd_decimal>() == value);
    CHECK(lit2.lexical_form() == std::to_string(value));

    value = -64524654389.2345650;
    auto lit3 = rdf4cpp::rdf::Literal::make<xsd::Decimal, xsd_decimal>(value);
    CHECK(lit3.value<xsd::Decimal, xsd_decimal>() == value);
    CHECK(lit3.lexical_form() == std::to_string(value));

    value = 1;
    auto lit4 = rdf4cpp::rdf::Literal{std::to_string(value), iri};
    CHECK(lit4.value<xsd::Decimal, xsd_decimal>() == value);

    value = 64582165456988.235046;
    auto lit5 = rdf4cpp::rdf::Literal{std::to_string(value), iri};
    CHECK(lit5.value<xsd::Decimal, xsd_decimal>() == value);

    CHECK(lit1 != lit2);
    CHECK(lit2 != lit3);
    CHECK(lit1 == lit4);
    CHECK(lit2 == lit5);

    auto lit6 = rdf4cpp::rdf::Literal{"NAN", iri};
    CHECK_THROWS_WITH_AS(lit6.value(), "XSD Parsing Error", std::runtime_error);

    auto lit7 = rdf4cpp::rdf::Literal{"INF", iri};
    CHECK_THROWS_WITH_AS(lit7.value(), "XSD Parsing Error", std::runtime_error);

    value = INFINITY;
    auto lit8 = rdf4cpp::rdf::Literal{std::to_string(value), iri};
    CHECK_THROWS_WITH_AS(lit8.value(), "XSD Parsing Error", std::runtime_error);

    value = NAN;
    auto lit9 = rdf4cpp::rdf::Literal{std::to_string(value), iri};
    CHECK_THROWS_WITH_AS(lit9.value(), "XSD Parsing Error", std::runtime_error);

    auto lit10 = rdf4cpp::rdf::Literal{"454sdsd", iri};
    CHECK_THROWS_WITH_AS(lit10.value(), "XSD Parsing Error", std::runtime_error);
}