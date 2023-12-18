#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("IRI") {
    IRIFactory f{};
    static constexpr std::string_view iri = "http//:example:foo/bar";

    datatypes::registry::dbpedia_mode = false;
    CHECK(!f.create_and_validate(iri).has_value());

    datatypes::registry::dbpedia_mode = true;
    CHECK(f.create_and_validate(iri).has_value());
    CHECK(f.create_and_validate(iri).value().identifier() == iri);
}

TEST_CASE("integers") {
    Literal u{};
    datatypes::registry::dbpedia_mode = false;
    CHECK_THROWS_AS(u = Literal::make_typed<datatypes::xsd::NonNegativeInteger>("-5"), std::runtime_error);
    CHECK_THROWS_AS(u = Literal::make_typed<datatypes::xsd::PositiveInteger>("0"), std::runtime_error);
    CHECK(u == Literal{});

    datatypes::registry::dbpedia_mode = true;
    CHECK(Literal::make_typed<datatypes::xsd::NonNegativeInteger>("-5") == Literal::make_typed<datatypes::xsd::NonNegativeInteger>("0"));
    CHECK(Literal::make_typed<datatypes::xsd::PositiveInteger>("0") == Literal::make_typed<datatypes::xsd::PositiveInteger>("1"));
}
