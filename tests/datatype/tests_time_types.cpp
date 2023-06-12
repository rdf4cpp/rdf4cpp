#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-shift-op-parentheses"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <format>
#include <rdf4cpp/rdf.hpp>

TEST_CASE("timezone") {
    using namespace rdf4cpp::rdf::datatypes::registry;
    Timezone tz{};
    tz.offset = std::chrono::minutes{60};
    Timezone zero_tz{};
    auto d = std::chrono::sys_days{std::chrono::day{1} / 2 / 2042} + std::chrono::hours{5} + std::chrono::minutes{30} + std::chrono::seconds{15};
    std::chrono::zoned_time d_in0{&zero_tz, d};
    std::chrono::zoned_time d_in60{&tz, d_in0};
    CHECK(std::format("{:%Y-%m-%d-%H-%M-%S-%z}", d_in60) == "2042-02-01-06-30-15-+0100");
}

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

TEST_CASE("datatype gYearMonth") {
    using namespace rdf4cpp::rdf;

    CHECK(std::string(datatypes::xsd::GYearMonth::identifier) == "http://www.w3.org/2001/XMLSchema#gYearMonth");

    rdf4cpp::rdf::datatypes::registry::OptionalTimezone tz = std::nullopt;
    CHECK(Literal::make_typed<datatypes::xsd::GYearMonth>("2042-5") == Literal::make_typed_from_value<datatypes::xsd::GYearMonth>(std::make_pair(std::chrono::year{2042} / 5, tz)));
    CHECK(Literal::make_typed<datatypes::xsd::GYearMonth>("2042-5") < Literal::make_typed_from_value<datatypes::xsd::GYearMonth>(std::make_pair(std::chrono::year{2042} / 6, tz)));
    CHECK(Literal::make_typed<datatypes::xsd::GYearMonth>("2042-5") < Literal::make_typed_from_value<datatypes::xsd::GYearMonth>(std::make_pair(std::chrono::year{2043} / 1, tz)));
    CHECK(Literal::make_typed_from_value<datatypes::xsd::GYearMonth>(std::make_pair(std::chrono::year{2042} / 5, tz)).lexical_form() == "2042-05");
}

#pragma clang diagnostic pop