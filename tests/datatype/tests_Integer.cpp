#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf::datatypes;

TEST_CASE("Datatype Integer") {

    auto iri = rdf4cpp::rdf::IRI(RegisteredDatatype<xsd::Integer, xsd_integer>::datatype_iri());

    auto iri_str = rdf4cpp::rdf::IRI("http://www.w3.org/2001/XMLSchema#integer");

    CHECK(iri == iri_str);

    int64_t value = 1;
    auto lit1 = rdf4cpp::rdf::Literal::make<xsd::Integer, xsd_integer>(value);
    CHECK(lit1.value<xsd::Integer, xsd_integer>() == value);
    CHECK(lit1.lexical_form() == std::to_string(value));

    value = -2147483648;
    auto lit2 = rdf4cpp::rdf::Literal::make<xsd::Integer, xsd_integer>(value);
    CHECK(lit2.value<xsd::Integer, xsd_integer>() == value);
    CHECK(lit2.lexical_form() == std::to_string(value));

    value = 2147483647;
    auto lit3 = rdf4cpp::rdf::Literal::make<xsd::Integer, xsd_integer>(value);
    CHECK(lit3.value<xsd::Integer, xsd_integer>() == value);
    CHECK(lit3.lexical_form() == std::to_string(value));

    //Testing Literal Constructor
    value = 1;
    auto lit4 = rdf4cpp::rdf::Literal{std::to_string(value), iri};
    CHECK(lit4.value<xsd::Integer, xsd_integer>() == value);

    value = 2147483647;
    auto lit5 = rdf4cpp::rdf::Literal{std::to_string(value), iri};
    CHECK(lit5.value<xsd::Integer, xsd_integer>() == value);

    /*    auto lit6 = rdf4cpp::rdf::Literal{"+1", iri};
    CHECK(lit6.value<xsd::Integer, xsd_integer>() == 1);*/

    CHECK(lit1 != lit2);
    CHECK(lit2 != lit3);
    CHECK(lit1 == lit4);
    CHECK(lit3 == lit5);
    //CHECK(lit4 == lit6);

    auto lit7 = rdf4cpp::rdf::Literal{"a23dg", iri};
    CHECK_THROWS_WITH_AS(lit7.value(), "XSD Parsing Error", std::runtime_error);
}