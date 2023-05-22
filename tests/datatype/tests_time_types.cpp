#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>


TEST_CASE("datatype gYear") {
    using namespace rdf4cpp::rdf;

    CHECK(std::string(datatypes::xsd::GYear::identifier) == "http://www.w3.org/2001/XMLSchema#gYear");

    CHECK(Literal::make_typed<datatypes::xsd::GYear>("500") == Literal::make_typed_from_value<datatypes::xsd::GYear>(date::year{500}));
    CHECK(Literal::make_typed<datatypes::xsd::GYear>("500") < Literal::make_typed_from_value<datatypes::xsd::GYear>(date::year{501}));
    CHECK(Literal::make_typed_from_value<datatypes::xsd::GYear>(date::year{500}).lexical_form() == "500");
}

TEST_CASE("datatype gMonth") {
    using namespace rdf4cpp::rdf;

    CHECK(std::string(datatypes::xsd::GMonth::identifier) == "http://www.w3.org/2001/XMLSchema#gMonth");

    CHECK(Literal::make_typed<datatypes::xsd::GMonth>("5") == Literal::make_typed_from_value<datatypes::xsd::GMonth>(date::May));
    CHECK(Literal::make_typed<datatypes::xsd::GMonth>("5") < Literal::make_typed_from_value<datatypes::xsd::GMonth>(date::July));
    CHECK(Literal::make_typed_from_value<datatypes::xsd::GMonth>(date::May).lexical_form() == "5");
}

TEST_CASE("datatype gDay") {
    using namespace rdf4cpp::rdf;

    CHECK(std::string(datatypes::xsd::GDay::identifier) == "http://www.w3.org/2001/XMLSchema#gDay");

    CHECK(Literal::make_typed<datatypes::xsd::GDay>("5") == Literal::make_typed_from_value<datatypes::xsd::GDay>(date::day{5}));
    CHECK(Literal::make_typed<datatypes::xsd::GDay>("5") < Literal::make_typed_from_value<datatypes::xsd::GDay>(date::day{6}));
    CHECK(Literal::make_typed_from_value<datatypes::xsd::GDay>(date::day{5}).lexical_form() == "5");
}
