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

TEST_CASE("date/time") {
    Literal u{};
    datatypes::registry::dbpedia_mode = false;
    CHECK_THROWS_AS(u = Literal::make_typed<datatypes::xsd::GDay>("---40"), std::runtime_error);
    CHECK_THROWS_AS(u = Literal::make_typed<datatypes::xsd::GMonth>("--40"), std::runtime_error);
    CHECK_THROWS_AS(u = Literal::make_typed<datatypes::xsd::GMonthDay>("--40-40"), std::runtime_error);
    CHECK_THROWS_AS(u = Literal::make_typed<datatypes::xsd::GYearMonth>("1742-0"), std::runtime_error);
    CHECK_THROWS_AS(u = Literal::make_typed<datatypes::xsd::Date>("1742-0-0"), std::runtime_error);
    CHECK_THROWS_AS(u = Literal::make_typed<datatypes::xsd::Date>("1742-2-40"), std::runtime_error);
    CHECK_THROWS_AS(u = Literal::make_typed<datatypes::xsd::Time>("10:90:00"), std::runtime_error);
    CHECK_THROWS_AS(u = Literal::make_typed<datatypes::xsd::Time>("26:90:60"), std::runtime_error);
    CHECK_THROWS_AS(u = Literal::make_typed<datatypes::xsd::DateTime>("1742-2-40T26:90:60"), std::runtime_error);
    CHECK_THROWS_AS(u = Literal::make_typed<datatypes::xsd::DateTimeStamp>("1742-2-40T26:90:60Z"), std::runtime_error);
    CHECK(u == Literal{});

    datatypes::registry::dbpedia_mode = true;
    CHECK(Literal::make_typed<datatypes::xsd::GDay>("---40") == Literal::make_typed<datatypes::xsd::GDay>("---31"));
    CHECK(Literal::make_typed<datatypes::xsd::GMonth>("--40") == Literal::make_typed<datatypes::xsd::GMonth>("--12"));
    CHECK(Literal::make_typed<datatypes::xsd::GMonthDay>("--40-40") == Literal::make_typed<datatypes::xsd::GMonthDay>("--12-31"));
    CHECK(Literal::make_typed<datatypes::xsd::GMonthDay>("--2-40") == Literal::make_typed<datatypes::xsd::GMonthDay>("--2-29"));
    CHECK(Literal::make_typed<datatypes::xsd::GYearMonth>("1742-0") == Literal::make_typed<datatypes::xsd::GYearMonth>("1742-1"));
    CHECK(Literal::make_typed<datatypes::xsd::Date>("1742-0-0") == Literal::make_typed<datatypes::xsd::Date>("1742-1-1"));
    CHECK(Literal::make_typed<datatypes::xsd::Date>("1742-2-40") == Literal::make_typed<datatypes::xsd::Date>("1742-2-28"));
    CHECK(Literal::make_typed<datatypes::xsd::Time>("10:90:00") == Literal::make_typed<datatypes::xsd::Time>("11:30:00"));
    CHECK(Literal::make_typed<datatypes::xsd::Time>("26:90:60") == Literal::make_typed<datatypes::xsd::Time>("23:59:59.999"));
    CHECK(Literal::make_typed<datatypes::xsd::DateTime>("1742-2-40T26:90:60") == Literal::make_typed<datatypes::xsd::DateTime>("1742-3-1T0:0:0"));
    CHECK(Literal::make_typed<datatypes::xsd::DateTimeStamp>("1742-2-40T26:90:60Z") == Literal::make_typed<datatypes::xsd::DateTimeStamp>("1742-3-1T0:0:0Z"));
    CHECK(Literal::make_typed<datatypes::xsd::DateTimeStamp>("1742-2-40T10:10:10Z") == Literal::make_typed<datatypes::xsd::DateTimeStamp>("1742-2-28T10:10:10Z"));
}
