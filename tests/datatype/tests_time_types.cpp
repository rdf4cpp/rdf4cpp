#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-shift-op-parentheses"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>


TEST_CASE("datatype gYear") {
    using namespace rdf4cpp::rdf;

    CHECK(std::string(datatypes::xsd::GYear::identifier) == "http://www.w3.org/2001/XMLSchema#gYear");

    CHECK(Literal::make_typed<datatypes::xsd::GYear>("500") == Literal::make_typed_from_value<datatypes::xsd::GYear>(std::chrono::year{500}));
    CHECK(Literal::make_typed<datatypes::xsd::GYear>("500") < Literal::make_typed_from_value<datatypes::xsd::GYear>(std::chrono::year{501}));
    CHECK(Literal::make_typed_from_value<datatypes::xsd::GYear>(std::chrono::year{500}).lexical_form() == "500");
}

TEST_CASE("datatype gMonth") {
    using namespace rdf4cpp::rdf;

    CHECK(std::string(datatypes::xsd::GMonth::identifier) == "http://www.w3.org/2001/XMLSchema#gMonth");

    CHECK(Literal::make_typed<datatypes::xsd::GMonth>("5") == Literal::make_typed_from_value<datatypes::xsd::GMonth>(std::chrono::May));
    CHECK(Literal::make_typed<datatypes::xsd::GMonth>("5") < Literal::make_typed_from_value<datatypes::xsd::GMonth>(std::chrono::July));
    CHECK(Literal::make_typed_from_value<datatypes::xsd::GMonth>(std::chrono::May).lexical_form() == "5");
}

TEST_CASE("datatype gDay") {
    using namespace rdf4cpp::rdf;

    CHECK(std::string(datatypes::xsd::GDay::identifier) == "http://www.w3.org/2001/XMLSchema#gDay");

    CHECK(Literal::make_typed<datatypes::xsd::GDay>("5") == Literal::make_typed_from_value<datatypes::xsd::GDay>(std::chrono::day{5}));
    CHECK(Literal::make_typed<datatypes::xsd::GDay>("5") < Literal::make_typed_from_value<datatypes::xsd::GDay>(std::chrono::day{6}));
    CHECK(Literal::make_typed_from_value<datatypes::xsd::GDay>(std::chrono::day{5}).lexical_form() == "5");
}

#pragma clang diagnostic pop