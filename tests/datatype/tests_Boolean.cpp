#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf::datatypes;

TEST_CASE("Datatype Boolean") {

    auto iri = rdf4cpp::rdf::IRI(RegisteredDatatype<xsd::Boolean, xsd_boolean>::datatype_iri());

    auto iri_str = rdf4cpp::rdf::IRI("http://www.w3.org/2001/XMLSchema#boolean");

    CHECK(iri == iri_str);

    bool value = true;
    auto lit1 = rdf4cpp::rdf::Literal::make<xsd::Boolean, xsd_boolean>(value);
    CHECK(lit1.value<xsd::Boolean, xsd_boolean>() == value);
    CHECK(lit1.lexical_form() == std::to_string(value));

    value = false;
    auto lit2 = rdf4cpp::rdf::Literal::make<xsd::Boolean, xsd_boolean>(value);
    CHECK(lit2.value<xsd::Boolean, xsd_boolean>() == value);
    CHECK(lit2.lexical_form() == std::to_string(value));

    value = 1;
    auto lit3 = rdf4cpp::rdf::Literal::make<xsd::Boolean, xsd_boolean>(value);
    CHECK(lit3.value<xsd::Boolean, xsd_boolean>() == value);
    CHECK(lit3.lexical_form() == std::to_string(value));

    value = 0;
    auto lit4 = rdf4cpp::rdf::Literal::make<xsd::Boolean, xsd_boolean>(value);
    CHECK(lit4.value<xsd::Boolean, xsd_boolean>() == value);

    value = true;
    auto lit5 = rdf4cpp::rdf::Literal{std::to_string(value), iri_str};
    CHECK(lit5.value<xsd::Boolean, xsd_boolean>() == value);

    value = false;
    auto lit6 = rdf4cpp::rdf::Literal{std::to_string(value), iri_str};
    CHECK(lit6.value<xsd::Boolean, xsd_boolean>() == value);

    CHECK(lit1 != lit2);
    CHECK(lit2 != lit3);
    CHECK(lit1 == lit3);
    CHECK(lit2 == lit4);
    CHECK(lit1 == lit5);
    CHECK(lit2 == lit6);
    CHECK(lit3 == lit5);
    CHECK(lit4 == lit6);

    auto lit7 = rdf4cpp::rdf::Literal{"5", iri};
    CHECK_THROWS_WITH_AS(lit7.value(), "XSD Parsing Error", std::runtime_error);

    auto lit8 = rdf4cpp::rdf::Literal{"adsfg", iri};
    CHECK_THROWS_WITH_AS(lit8.value(), "XSD Parsing Error", std::runtime_error);

    auto lit9 = rdf4cpp::rdf::Literal{"5.64566", iri};
    CHECK_THROWS_WITH_AS(lit9.value(), "XSD Parsing Error", std::runtime_error);

    auto lit10 = rdf4cpp::rdf::Literal{"1.7e", iri};
    CHECK_THROWS_WITH_AS(lit10.value(), "XSD Parsing Error", std::runtime_error);
}