#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-shift-op-parentheses"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <format>
#include <rdf4cpp/rdf.hpp>

template<class Datatype, class Compatible>
void basic_test(Compatible a, std::string_view b, std::partial_ordering res) {
    using namespace rdf4cpp::rdf;
    // better output on failure than CHECK(a <=> b == res)
    if (res == std::partial_ordering::equivalent) {
        CHECK(Literal::make_typed_from_value<Datatype>(a).lexical_form() == b);
        CHECK(Literal::make_typed_from_value<Datatype>(a) == Literal::make_typed<Datatype>(b));
    }
    else if (res == std::partial_ordering::less) {
        CHECK(Literal::make_typed_from_value<Datatype>(a) < Literal::make_typed<Datatype>(b));
    }
    else if (res == std::partial_ordering::greater) {
        CHECK(Literal::make_typed_from_value<Datatype>(a) > Literal::make_typed<Datatype>(b));
    }
}

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

    rdf4cpp::rdf::datatypes::registry::OptionalTimezone tz = std::nullopt;
    basic_test<datatypes::xsd::GYear>(std::make_pair(std::chrono::year{500}, tz), "0500", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GYear>(std::make_pair(std::chrono::year{500}, tz), "0501", std::partial_ordering::less);
    basic_test<datatypes::xsd::GYear>(std::make_pair(std::chrono::year{500}, datatypes::registry::Timezone{std::chrono::hours{1}}), "0500+1:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GYear>(std::make_pair(std::chrono::year{500}, datatypes::registry::Timezone{std::chrono::minutes{-65}}), "0500-1:05", std::partial_ordering::equivalent);
}

TEST_CASE("datatype gMonth") {
    using namespace rdf4cpp::rdf;

    CHECK(std::string(datatypes::xsd::GMonth::identifier) == "http://www.w3.org/2001/XMLSchema#gMonth");

    rdf4cpp::rdf::datatypes::registry::OptionalTimezone tz = std::nullopt;
    CHECK(Literal::make_typed<datatypes::xsd::GMonth>("5") == Literal::make_typed_from_value<datatypes::xsd::GMonth>(std::make_pair(std::chrono::May, tz)));
    CHECK(Literal::make_typed<datatypes::xsd::GMonth>("5") < Literal::make_typed_from_value<datatypes::xsd::GMonth>(std::make_pair(std::chrono::July, tz)));
    CHECK(Literal::make_typed_from_value<datatypes::xsd::GMonth>(std::make_pair(std::chrono::May, tz)).lexical_form() == "05");
    CHECK(Literal::make_typed_from_value<datatypes::xsd::GMonth>(std::make_pair(std::chrono::May, datatypes::registry::Timezone{std::chrono::hours{1}})).lexical_form() == "05+1:00");
    CHECK(Literal::make_typed_from_value<datatypes::xsd::GMonth>(std::make_pair(std::chrono::May, datatypes::registry::Timezone{std::chrono::minutes{-65}})).lexical_form() == "05-1:05");
}

TEST_CASE("datatype gDay") {
    using namespace rdf4cpp::rdf;

    CHECK(std::string(datatypes::xsd::GDay::identifier) == "http://www.w3.org/2001/XMLSchema#gDay");

    rdf4cpp::rdf::datatypes::registry::OptionalTimezone tz = std::nullopt;
    CHECK(Literal::make_typed<datatypes::xsd::GDay>("5") == Literal::make_typed_from_value<datatypes::xsd::GDay>(std::make_pair(std::chrono::day{5}, tz)));
    CHECK(Literal::make_typed<datatypes::xsd::GDay>("5") < Literal::make_typed_from_value<datatypes::xsd::GDay>(std::make_pair(std::chrono::day{6}, tz)));
    CHECK(Literal::make_typed_from_value<datatypes::xsd::GDay>(std::make_pair(std::chrono::day{5}, tz)).lexical_form() == "05");
    CHECK(Literal::make_typed_from_value<datatypes::xsd::GDay>(std::make_pair(std::chrono::day{5}, datatypes::registry::Timezone{std::chrono::hours{1}})).lexical_form() == "05+1:00");
    CHECK(Literal::make_typed_from_value<datatypes::xsd::GDay>(std::make_pair(std::chrono::day{5}, datatypes::registry::Timezone{std::chrono::minutes{-65}})).lexical_form() == "05-1:05");
}

TEST_CASE("datatype gYearMonth") {
    using namespace rdf4cpp::rdf;

    CHECK(std::string(datatypes::xsd::GYearMonth::identifier) == "http://www.w3.org/2001/XMLSchema#gYearMonth");

    rdf4cpp::rdf::datatypes::registry::OptionalTimezone tz = std::nullopt;
    CHECK(Literal::make_typed<datatypes::xsd::GYearMonth>("2042-5") == Literal::make_typed_from_value<datatypes::xsd::GYearMonth>(std::make_pair(std::chrono::year{2042} / 5, tz)));
    CHECK(Literal::make_typed<datatypes::xsd::GYearMonth>("2042-5") < Literal::make_typed_from_value<datatypes::xsd::GYearMonth>(std::make_pair(std::chrono::year{2042} / 6, tz)));
    CHECK(Literal::make_typed<datatypes::xsd::GYearMonth>("2042-5") < Literal::make_typed_from_value<datatypes::xsd::GYearMonth>(std::make_pair(std::chrono::year{2043} / 1, tz)));
    CHECK(Literal::make_typed_from_value<datatypes::xsd::GYearMonth>(std::make_pair(std::chrono::year{2042} / 5, tz)).lexical_form() == "2042-05");
    CHECK(Literal::make_typed_from_value<datatypes::xsd::GYearMonth>(std::make_pair(std::chrono::year{2042} / 5, datatypes::registry::Timezone{std::chrono::hours{1}})).lexical_form() == "2042-05+1:00");
    CHECK(Literal::make_typed_from_value<datatypes::xsd::GYearMonth>(std::make_pair(std::chrono::year{2042} / 5, datatypes::registry::Timezone{std::chrono::minutes{-65}})).lexical_form() == "2042-05-1:05");
}

#pragma clang diagnostic pop