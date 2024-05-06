#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <format>
#include <rdf4cpp.hpp>
#include <rdf4cpp/storage/reference_node_storage/SyncReferenceNodeStorage.hpp>

#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>
#include <random>

template<typename Datatype>
void basic_test(typename Datatype::cpp_type a, std::string_view b, std::partial_ordering res, bool skip_string_comp = false) {
    using namespace rdf4cpp;
    // better output on failure than CHECK(a <=> b == res)
    if (res == std::partial_ordering::equivalent) {
        const auto lit_a = Literal::make_typed_from_value<Datatype>(a);
        const auto lit_b = Literal::make_typed<Datatype>(b);
        if (!skip_string_comp)
            CHECK(lit_a.lexical_form() == b);
        CHECK(lit_a == lit_b);
        CHECK(a == lit_b.template value<Datatype>());
    } else if (res == std::partial_ordering::less) {
        CHECK(Literal::make_typed_from_value<Datatype>(a) < Literal::make_typed<Datatype>(b));
    }
    else if (res == std::partial_ordering::greater) {
        CHECK(Literal::make_typed_from_value<Datatype>(a) > Literal::make_typed<Datatype>(b));
    }
    else if (res == std::partial_ordering::unordered) {
        CHECK(Literal::make_typed_from_value<Datatype>(a) <=> Literal::make_typed<Datatype>(b) == res);
    }
}
template<typename Datatype>
void basic_test(std::string_view a, std::string_view b, std::partial_ordering res) {
    using namespace rdf4cpp;
    // better output on failure than CHECK(a <=> b == res)
    if (res == std::partial_ordering::equivalent) {
        const auto lit_a = Literal::make_typed<Datatype>(a);
        const auto lit_b = Literal::make_typed<Datatype>(b);
        CHECK(lit_a == lit_b);
    } else if (res == std::partial_ordering::less) {
        CHECK(Literal::make_typed<Datatype>(a) < Literal::make_typed<Datatype>(b));
    }
    else if (res == std::partial_ordering::greater) {
        CHECK(Literal::make_typed<Datatype>(a) > Literal::make_typed<Datatype>(b));
    }
    else if (res == std::partial_ordering::unordered) {
        CHECK(Literal::make_typed<Datatype>(a) <=> Literal::make_typed<Datatype>(b) == res);
    }
}

TEST_CASE("timezone") {
    using namespace rdf4cpp::datatypes::registry;
    rdf4cpp::Timezone tz_60{std::chrono::minutes{60}};
    rdf4cpp::Timezone tz_0{};
    auto d = std::chrono::sys_days{std::chrono::day{1} / 2 / 2042} + std::chrono::hours{5} + std::chrono::minutes{30} + std::chrono::seconds{15};
    std::chrono::zoned_time d_in0{tz_0, d};
    std::chrono::zoned_time d_in60{tz_60, d_in0};
    CHECK(std::format("{:%Y-%m-%d-%H-%M-%S-%Z}", d_in60) == "2042-02-01-06-30-15-+01:00");
    CHECK(std::format("{:%Y-%m-%d-%H-%M-%S-%Z}", d_in0) == "2042-02-01-05-30-15-Z");

    auto l = std::chrono::local_days{std::chrono::day{1} / 2 / 2042} + std::chrono::hours{5} + std::chrono::minutes{30} + std::chrono::seconds{15};
    d_in60 = std::chrono::zoned_time(tz_60, l);
    d_in0 = std::chrono::zoned_time(tz_0, d_in60);
    CHECK(std::format("{:%Y-%m-%d-%H-%M-%S-%Z}", d_in60) == "2042-02-01-05-30-15-+01:00");
    CHECK(std::format("{:%Y-%m-%d-%H-%M-%S-%Z}", d_in0) == "2042-02-01-04-30-15-Z");
}

TEST_CASE("precision") {
    auto ms = std::chrono::microseconds::max();
    auto ys = std::chrono::floor<std::chrono::years>(ms);
    CHECK(ys > std::chrono::years{10000});
    CHECK(ys > std::chrono::years{static_cast<int>(std::chrono::year::max())});
}

TEST_CASE("datatype gYear") {
    using namespace rdf4cpp;

    CHECK(std::string(datatypes::xsd::GYear::identifier) == "http://www.w3.org/2001/XMLSchema#gYear");

    rdf4cpp::OptionalTimezone tz = std::nullopt;
    basic_test<datatypes::xsd::GYear>(std::make_pair(std::chrono::year{500}, tz), "0500", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GYear>(std::make_pair(std::chrono::year{500}, tz), "0501", std::partial_ordering::less);
    basic_test<datatypes::xsd::GYear>(std::make_pair(std::chrono::year{500}, Timezone{std::chrono::hours{1}}), "0500+01:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GYear>(std::make_pair(std::chrono::year{500}, Timezone{std::chrono::minutes{-65}}), "0500-01:05", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GYear>(std::make_pair(std::chrono::year{500}, Timezone{std::chrono::hours{-14}}), "0500-14:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GYear>(std::make_pair(std::chrono::year{500}, Timezone{std::chrono::hours{14}}), "0500+14:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GYear>(std::make_pair(std::chrono::year::min(), Timezone{std::chrono::hours{-14}}), "-32767-14:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GYear>(std::make_pair(std::chrono::year::max(), Timezone{std::chrono::hours{14}}), "32767+14:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GYear>("0500-1:00", "0500Z", std::partial_ordering::greater);
    basic_test<datatypes::xsd::GYear>("0500+1:00", "0500Z", std::partial_ordering::less);
    CHECK(Literal::make_typed<datatypes::xsd::GYear>("05").is_inlined());
    CHECK(Literal::make_typed<datatypes::xsd::GYear>("12").is_inlined());
    CHECK(Literal::make_typed<datatypes::xsd::GYear>("12+14:00").is_inlined());
    CHECK(Literal::make_typed<datatypes::xsd::GYear>("12-14:00").is_inlined());
}

TEST_CASE("datatype gMonth") {
    using namespace rdf4cpp;

    CHECK(std::string(datatypes::xsd::GMonth::identifier) == "http://www.w3.org/2001/XMLSchema#gMonth");

    rdf4cpp::OptionalTimezone tz = std::nullopt;
    basic_test<datatypes::xsd::GMonth>(std::make_pair(std::chrono::May, tz), "--05", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GMonth>(std::make_pair(std::chrono::April, tz), "--05", std::partial_ordering::less);
    basic_test<datatypes::xsd::GMonth>(std::make_pair(std::chrono::May, Timezone{std::chrono::hours{1}}), "--05+01:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GMonth>(std::make_pair(std::chrono::May, Timezone{std::chrono::minutes{-65}}), "--05-01:05", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GMonth>(std::make_pair(std::chrono::May, Timezone{std::chrono::hours{-14}}), "--05-14:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GMonth>(std::make_pair(std::chrono::May, Timezone{std::chrono::hours{14}}), "--05+14:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GMonth>(std::make_pair(std::chrono::January, Timezone{std::chrono::hours{-14}}), "--01-14:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GMonth>(std::make_pair(std::chrono::December, Timezone{std::chrono::hours{14}}), "--12+14:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GMonth>("--05+1:00", "--05Z", std::partial_ordering::less);
    basic_test<datatypes::xsd::GMonth>("--05-1:00", "--05Z", std::partial_ordering::greater);
    CHECK(Literal::make_typed<datatypes::xsd::GMonth>("--05").is_inlined());
    CHECK(Literal::make_typed<datatypes::xsd::GMonth>("--12").is_inlined());
    CHECK(Literal::make_typed<datatypes::xsd::GMonth>("--12+14:00").is_inlined());
    CHECK(Literal::make_typed<datatypes::xsd::GMonth>("--12-14:00").is_inlined());
    Literal a{};
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::GMonth>("00"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::GMonth>("13"), std::runtime_error);
    CHECK(a == Literal{}); // turn off unused and nodiscard ignored warnings
}

TEST_CASE("datatype gDay") {
    using namespace rdf4cpp;

    CHECK(std::string(datatypes::xsd::GDay::identifier) == "http://www.w3.org/2001/XMLSchema#gDay");

    rdf4cpp::OptionalTimezone tz = std::nullopt;
    basic_test<datatypes::xsd::GDay>(std::make_pair(std::chrono::day{5}, tz), "---05", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GDay>(std::make_pair(std::chrono::day{4}, tz), "---05", std::partial_ordering::less);
    basic_test<datatypes::xsd::GDay>(std::make_pair(std::chrono::day{5}, Timezone{std::chrono::hours{1}}), "---05+01:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GDay>(std::make_pair(std::chrono::day{5}, Timezone{std::chrono::minutes{-65}}), "---05-01:05", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GDay>(std::make_pair(std::chrono::day{1}, Timezone{std::chrono::hours{-14}}), "---01-14:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GDay>(std::make_pair(std::chrono::day{31}, Timezone{std::chrono::hours{14}}), "---31+14:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GDay>("---05+1:00", "---05Z", std::partial_ordering::less);
    basic_test<datatypes::xsd::GDay>("---05-1:00", "---05Z", std::partial_ordering::greater);
    CHECK(Literal::make_typed<datatypes::xsd::GDay>("---05").is_inlined());
    CHECK(Literal::make_typed<datatypes::xsd::GDay>("---31").is_inlined());
    CHECK(Literal::make_typed<datatypes::xsd::GDay>("---31+14:00").is_inlined());
    CHECK(Literal::make_typed<datatypes::xsd::GDay>("---31-14:00").is_inlined());
    Literal a{};
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::GDay>("---00"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::GDay>("---32"), std::runtime_error);
    CHECK(a == Literal{}); // turn off unused and nodiscard ignored warnings
}

TEST_CASE("datatype gYearMonth") {
    using namespace rdf4cpp;

    CHECK(std::string(datatypes::xsd::GYearMonth::identifier) == "http://www.w3.org/2001/XMLSchema#gYearMonth");

    rdf4cpp::OptionalTimezone tz = std::nullopt;
    basic_test<datatypes::xsd::GYearMonth>(std::make_pair(std::chrono::year{2042} / 5, tz), "2042-05", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GYearMonth>(std::make_pair(std::chrono::year{2042} / 4, tz), "2042-05", std::partial_ordering::less);
    basic_test<datatypes::xsd::GYearMonth>(std::make_pair(std::chrono::year{2041} / 6, tz), "2042-05", std::partial_ordering::less);
    basic_test<datatypes::xsd::GYearMonth>(std::make_pair(std::chrono::year{2042} / 5, Timezone{std::chrono::hours{1}}), "2042-05+01:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GYearMonth>(std::make_pair(std::chrono::year{2042} / 5, Timezone{std::chrono::minutes{-65}}), "2042-05-01:05", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GYearMonth>(std::make_pair(std::chrono::year{2042} / 5, Timezone{std::chrono::hours{-14}}), "2042-05-14:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GYearMonth>(std::make_pair(std::chrono::year{2042} / 5, Timezone{std::chrono::hours{14}}), "2042-05+14:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GYearMonth>(std::make_pair(std::chrono::year::max() / 12, tz), "32767-12", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GYearMonth>(std::make_pair(std::chrono::year::min() / 1, tz), "-32767-01", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GYearMonth>("2042-05+1:00", "2042-05Z", std::partial_ordering::less);
    basic_test<datatypes::xsd::GYearMonth>("2042-05-1:00", "2042-05Z", std::partial_ordering::greater);
    CHECK(Literal::make_typed<datatypes::xsd::GYearMonth>("2042-05").is_inlined());
    CHECK(Literal::make_typed<datatypes::xsd::GYearMonth>("32767-12").is_inlined());
    CHECK(Literal::make_typed<datatypes::xsd::GYearMonth>("-32767-1").is_inlined());
    CHECK(!Literal::make_typed<datatypes::xsd::GYearMonth>("-32767-1-14:00").is_inlined());
    CHECK(storage::reference_node_storage::SyncReferenceNodeStorage::has_specialized_storage_for(datatypes::xsd::GYearMonth::fixed_id));
    Literal a{};
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::GYearMonth>("01-00"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::GYearMonth>("-32768-01"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::GYearMonth>("32767-32"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::GYearMonth>("32768-30"), std::runtime_error);
    CHECK(a == Literal{}); // turn off unused and nodiscard ignored warnings
}

TEST_CASE("datatype gMonthDay") {
    using namespace rdf4cpp;

    CHECK(std::string(datatypes::xsd::GMonthDay::identifier) == "http://www.w3.org/2001/XMLSchema#gMonthDay");

    rdf4cpp::OptionalTimezone tz = std::nullopt;
    basic_test<datatypes::xsd::GMonthDay>(std::make_pair(std::chrono::month{5} / 5, tz), "--05-05", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GMonthDay>(std::make_pair(std::chrono::month{5} / 4, tz), "--05-05", std::partial_ordering::less);
    basic_test<datatypes::xsd::GMonthDay>(std::make_pair(std::chrono::month{3} / 6, tz), "--04-05", std::partial_ordering::less);
    basic_test<datatypes::xsd::GMonthDay>(std::make_pair(std::chrono::month{5} / 5, Timezone{std::chrono::hours{1}}), "--05-05+01:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GMonthDay>(std::make_pair(std::chrono::month{5} / 5, Timezone{std::chrono::minutes{-65}}), "--05-05-01:05", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GMonthDay>(std::make_pair(std::chrono::month{1} / 1, Timezone{std::chrono::hours{-14}}), "--01-01-14:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GMonthDay>(std::make_pair(std::chrono::month{12} / 31, Timezone{std::chrono::hours{14}}), "--12-31+14:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::GMonthDay>("--05-05+1:00", "--05-05Z", std::partial_ordering::less);
    basic_test<datatypes::xsd::GMonthDay>("--05-05-1:00", "--05-05Z", std::partial_ordering::greater);
    CHECK(Literal::make_typed<datatypes::xsd::GMonthDay>("--05-1").is_inlined());
    CHECK(Literal::make_typed<datatypes::xsd::GMonthDay>("--12-31").is_inlined());
    CHECK(Literal::make_typed<datatypes::xsd::GMonthDay>("--12-31+14:00").is_inlined());
    CHECK(Literal::make_typed<datatypes::xsd::GMonthDay>("--12-31-14:00").is_inlined());
    Literal a{};
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::GMonthDay>("--01-00"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::GMonthDay>("--00-01"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::GMonthDay>("--12-32"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::GMonthDay>("--13-30"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::GMonthDay>("--02-30"), std::runtime_error);
    CHECK(a == Literal{}); // turn off unused and nodiscard ignored warnings
}

TEST_CASE("datatype date") {
    using namespace rdf4cpp;

    CHECK(std::string(datatypes::xsd::Date::identifier) == "http://www.w3.org/2001/XMLSchema#date");

    rdf4cpp::OptionalTimezone tz = std::nullopt;
    basic_test<datatypes::xsd::Date>(std::make_pair(std::chrono::year{2042} / 5 / 1, tz), "2042-05-01", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::Date>(std::make_pair(std::chrono::year{2042} / 4 / 1, tz), "2042-05-01", std::partial_ordering::less);
    basic_test<datatypes::xsd::Date>(std::make_pair(std::chrono::year{2041} / 6 / 1, tz), "2042-05-01", std::partial_ordering::less);
    basic_test<datatypes::xsd::Date>(std::make_pair(std::chrono::year{2042} / 5 / 1, Timezone{std::chrono::hours{1}}), "2042-05-01+01:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::Date>(std::make_pair(std::chrono::year{2042} / 5 / 1, Timezone{std::chrono::minutes{-65}}), "2042-05-01-01:05", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::Date>(std::make_pair(std::chrono::year::min() / 1 / 1, Timezone{std::chrono::hours{-14}}), "-32767-01-01-14:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::Date>(std::make_pair(std::chrono::year::max() / 12 / 31, Timezone{std::chrono::hours{14}}), "32767-12-31+14:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::Date>(std::make_pair(std::chrono::year::min() / 1 / 1, tz), "-32767-01-01", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::Date>(std::make_pair(std::chrono::year::max() / 12 / 31, tz), "32767-12-31", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::Date>("2042-05-01+1:00", "2042-05-01Z", std::partial_ordering::less);
    basic_test<datatypes::xsd::Date>("2042-05-01-1:00", "2042-05-01Z", std::partial_ordering::greater);
    CHECK(Literal::make_typed<datatypes::xsd::Date>("2042-05-1").is_inlined());
    CHECK(Literal::make_typed<datatypes::xsd::Date>("2042-12-31").is_inlined());
    CHECK(!Literal::make_typed<datatypes::xsd::Date>("2042-12-31+14:00").is_inlined());
    CHECK(!Literal::make_typed<datatypes::xsd::Date>("2042-12-31-14:00").is_inlined());
    CHECK(storage::default_node_storage.has_specialized_storage_for(datatypes::xsd::Date::fixed_id));
    Literal a{};
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::Date>("2042-01-00"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::Date>("2042-00-01"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::Date>("2042-12-32"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::Date>("2042-13-30"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::Date>("2042-02-30"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::Date>("1937-0-0"), std::runtime_error);
    CHECK(a == Literal{}); // turn off unused and nodiscard ignored warnings
}

TEST_CASE("datatype time") {
    using namespace rdf4cpp;

    CHECK(std::string(datatypes::xsd::Time::identifier) == "http://www.w3.org/2001/XMLSchema#time");

    rdf4cpp::OptionalTimezone tz = std::nullopt;
    basic_test<datatypes::xsd::Time>(std::make_pair(std::chrono::minutes{50}, tz), "00:50:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::Time>(std::make_pair(std::chrono::hours{12} + std::chrono::minutes{34} + std::chrono::seconds{56} + std::chrono::milliseconds{789}, tz), "12:34:56.789", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::Time>(std::make_pair(std::chrono::minutes{50} + std::chrono::milliseconds{100}, tz), "00:50:00.1", std::partial_ordering::equivalent, true);
    basic_test<datatypes::xsd::Time>(std::make_pair(std::chrono::minutes{50} + std::chrono::milliseconds{123}, tz), "00:50:00.12345", std::partial_ordering::equivalent, true);
    basic_test<datatypes::xsd::Time>(std::make_pair(std::chrono::minutes{42}, tz), "00:50:00", std::partial_ordering::less);
    basic_test<datatypes::xsd::Time>(std::make_pair(std::chrono::minutes{50}, Timezone{std::chrono::hours{1}}), "00:50:00+01:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::Time>(std::make_pair(std::chrono::minutes{50}, Timezone{std::chrono::minutes{-65}}), "00:50:00-01:05", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::Time>(std::make_pair(std::chrono::hours{0}, Timezone{std::chrono::hours{-14}}), "00:00:00-14:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::Time>(std::make_pair(std::chrono::hours{24}, Timezone{std::chrono::hours{14}}), "24:00:00+14:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::Time>("00:50:00+1:00", "00:50:00Z", std::partial_ordering::less);
    basic_test<datatypes::xsd::Time>("00:50:00-1:00", "00:50:00Z", std::partial_ordering::greater);
    CHECK(Literal::make_typed<datatypes::xsd::Time>("00:00:00.000").is_inlined());
    CHECK(Literal::make_typed<datatypes::xsd::Time>("24:00:00.000").is_inlined());
    CHECK(Literal::make_typed<datatypes::xsd::Time>("24:00:00.000+14:00").is_inlined());
    CHECK(Literal::make_typed<datatypes::xsd::Time>("00:00:00.000-14:00").is_inlined());
    Literal a{};
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::Time>("-10:00:00.000"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::Time>("25:00:00.000"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::Time>("00:-1:00.000"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::Time>("00:70:00.000"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::Time>("00:00:-1.000"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::Time>("00:00:70.000"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::Time>("00:00:5.-100"), std::runtime_error);
    CHECK(a == Literal{}); // turn off unused and nodiscard ignored warnings
}

TEST_CASE("datatype dateTime") {
    using namespace rdf4cpp;

    CHECK(std::string(datatypes::xsd::DateTime::identifier) == "http://www.w3.org/2001/XMLSchema#dateTime");

    rdf4cpp::OptionalTimezone tz = std::nullopt;
    basic_test<datatypes::xsd::DateTime>(std::make_pair(rdf4cpp::util::construct_timepoint(std::chrono::year{2042} / 5 / 1, std::chrono::minutes{50}), tz), "2042-05-01T00:50:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::DateTime>(std::make_pair(rdf4cpp::util::construct_timepoint(std::chrono::year{2042} / 5 / 1, std::chrono::hours{12} + std::chrono::minutes{34} + std::chrono::seconds{56} + std::chrono::milliseconds{789}), tz), "2042-05-01T12:34:56.789", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::DateTime>(std::make_pair(rdf4cpp::util::construct_timepoint(std::chrono::year{2042} / 5 / 1, std::chrono::minutes{50} + std::chrono::milliseconds{100}), tz), "2042-05-01T00:50:00.1", std::partial_ordering::equivalent, true);
    basic_test<datatypes::xsd::DateTime>(std::make_pair(rdf4cpp::util::construct_timepoint(std::chrono::year{2042} / 5 / 1, std::chrono::minutes{50} + std::chrono::milliseconds{123}), tz), "2042-05-01T00:50:00.12345", std::partial_ordering::equivalent, true);
    basic_test<datatypes::xsd::DateTime>(std::make_pair(rdf4cpp::util::construct_timepoint(std::chrono::year{2042} / 5 / 1, std::chrono::minutes{42}), tz), "2042-05-01T00:50:00", std::partial_ordering::less);
    basic_test<datatypes::xsd::DateTime>(std::make_pair(rdf4cpp::util::construct_timepoint(std::chrono::year{2042} / 5 / 1, std::chrono::minutes{50}), Timezone{std::chrono::hours{1}}), "2042-05-01T00:50:00+01:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::DateTime>(std::make_pair(rdf4cpp::util::construct_timepoint(std::chrono::year{2042} / 5 / 1, std::chrono::minutes{50}), Timezone{std::chrono::minutes{-65}}), "2042-05-01T00:50:00-01:05", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::DateTime>("2042-05-05T13:40:08", "2042-05-05T13:40:08", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::DateTime>("2041-05-05T13:40:08", "2042-05-05T13:40:08", std::partial_ordering::less);
    basic_test<datatypes::xsd::DateTime>("2042-05-05T13:40:08", "2041-05-05T13:40:08", std::partial_ordering::greater);
    basic_test<datatypes::xsd::DateTime>("2042-04-05T13:40:08", "2042-05-05T13:40:08", std::partial_ordering::less);
    basic_test<datatypes::xsd::DateTime>("2042-05-05T13:40:08", "2042-04-05T13:40:08", std::partial_ordering::greater);
    basic_test<datatypes::xsd::DateTime>("2042-05-04T13:40:08", "2042-05-05T13:40:08", std::partial_ordering::less);
    basic_test<datatypes::xsd::DateTime>("2042-05-05T13:40:08", "2042-05-04T13:40:08", std::partial_ordering::greater);
    basic_test<datatypes::xsd::DateTime>("2042-05-05T12:40:08", "2042-05-05T13:40:08", std::partial_ordering::less);
    basic_test<datatypes::xsd::DateTime>("2042-05-05T13:40:08", "2042-05-05T12:40:08", std::partial_ordering::greater);
    basic_test<datatypes::xsd::DateTime>("2042-05-05T13:39:08", "2042-05-05T13:40:08", std::partial_ordering::less);
    basic_test<datatypes::xsd::DateTime>("2042-05-05T13:40:08", "2042-05-05T13:39:08", std::partial_ordering::greater);
    basic_test<datatypes::xsd::DateTime>("2042-05-05T13:40:07", "2042-05-05T13:40:08", std::partial_ordering::less);
    basic_test<datatypes::xsd::DateTime>("2042-05-05T13:40:08", "2042-05-05T13:40:07", std::partial_ordering::greater);
    basic_test<datatypes::xsd::DateTime>("2042-05-05T13:40:08.005", "2042-05-05T13:40:08.006", std::partial_ordering::less);
    basic_test<datatypes::xsd::DateTime>("2042-05-05T13:40:08.006", "2042-05-05T13:40:08.005", std::partial_ordering::greater);
    basic_test<datatypes::xsd::DateTime>("2042-05-05T13:40:08Z", "2042-05-05T14:40:08+01:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::DateTime>("2042-05-05T13:40:08Z", "2042-05-05T12:30:08-01:10", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::DateTime>("2042-05-05T13:40:08Z", "2042-05-05T13:40:08", std::partial_ordering::unordered);
    basic_test<datatypes::xsd::DateTime>("2042-05-05T13:40:08Z", "2043-05-05T13:40:08", std::partial_ordering::less);
    basic_test<datatypes::xsd::DateTime>("2042-05-05T13:40:08Z", "2041-05-05T13:40:08", std::partial_ordering::greater);
    basic_test<datatypes::xsd::DateTime>("2042-05-05T13:40:08", "2043-05-05T13:40:08Z", std::partial_ordering::less);
    basic_test<datatypes::xsd::DateTime>("2042-05-05T13:40:08", "2041-05-05T13:40:08Z", std::partial_ordering::greater);
    basic_test<datatypes::xsd::DateTime>(std::make_pair(rdf4cpp::util::construct_timepoint(std::chrono::year::min() / 1 / 1, std::chrono::hours{0}), Timezone{std::chrono::hours{-14}}), "-32767-01-01T00:00:00-14:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::DateTime>(std::make_pair(rdf4cpp::util::construct_timepoint( std::chrono::year::max() / 12 / 31,std::chrono::hours{24} - std::chrono::milliseconds{1}), Timezone{std::chrono::hours{14}}), "32767-12-31T23:59:59.999+14:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::DateTime>(std::make_pair(rdf4cpp::util::construct_timepoint(std::chrono::year::min() / 1 / 1, std::chrono::hours{0}), tz), "-32767-01-01T00:00:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::DateTime>(std::make_pair(rdf4cpp::util::construct_timepoint(std::chrono::year::max() / 12 / 31,std::chrono::hours{24} - std::chrono::milliseconds{1}), tz), "32767-12-31T23:59:59.999", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::DateTime>(std::make_pair(rdf4cpp::util::construct_timepoint(std::chrono::year::max() / 12 / 31,std::chrono::hours{24} - std::chrono::seconds{1}), tz), "32767-12-31T23:59:59", std::partial_ordering::equivalent);
    CHECK(Literal::make_typed<datatypes::xsd::DateTime>("-32767-01-01T00:00:00.000").is_inlined());
    CHECK(Literal::make_typed<datatypes::xsd::DateTime>("32767-12-31T23:59:59.000").is_inlined());
    CHECK(!Literal::make_typed<datatypes::xsd::DateTime>("-32767-01-01T00:00:00.000-14:00").is_inlined());
    CHECK(!Literal::make_typed<datatypes::xsd::DateTime>("32767-12-31T23:59:59.999+14:00").is_inlined());
    CHECK(storage::default_node_storage.has_specialized_storage_for(datatypes::xsd::DateTime::fixed_id));
    Literal a{};
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DateTime>("2042-05-05T-10:00:00.000"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DateTime>("2042-05-05T25:00:00.000"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DateTime>("2042-05-05T00:-1:00.000"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DateTime>("2042-05-05T00:70:00.000"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DateTime>("2042-05-05T00:00:-1.000"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DateTime>("2042-05-05T00:00:70.000"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DateTime>("2042-05-05T00:00:5.-100"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DateTime>("2042-01-00T00:00:00.000"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DateTime>("2042-00-01T00:00:00.000"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DateTime>("2042-12-32T00:00:00.000"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DateTime>("2042-13-30T00:00:00.000"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DateTime>("2042-02-30T00:00:00.000"), std::runtime_error);
    CHECK(a == Literal{}); // turn off unused and nodiscard ignored warnings
    CHECK(Literal::make_typed<datatypes::xsd::DateTime>("2042-05-06T00:00:00.000") == Literal::make_typed<datatypes::xsd::DateTime>("2042-05-05T24:00:00"));
    CHECK(Literal::make_typed<datatypes::xsd::DateTime>("2042-05-05T24:00:00.000").lexical_form() == "2042-05-06T00:00:00");
}

TEST_CASE("datatype dateTimeStamp") {
    using namespace rdf4cpp;

    CHECK(std::string(datatypes::xsd::DateTimeStamp::identifier) == "http://www.w3.org/2001/XMLSchema#dateTimeStamp");

    Timezone tz{std::chrono::hours{0}};
    basic_test<datatypes::xsd::DateTimeStamp>(ZonedTime(tz, util::construct_timepoint(std::chrono::year{2042} / 5 / 1, std::chrono::minutes{50})), "2042-05-01T00:50:00Z", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::DateTimeStamp>(ZonedTime(tz, util::construct_timepoint(std::chrono::year{2042} / 5 / 1, std::chrono::hours{12} + std::chrono::minutes{34} + std::chrono::seconds{56} + std::chrono::milliseconds{789})), "2042-05-01T12:34:56.789Z", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::DateTimeStamp>(ZonedTime(tz, util::construct_timepoint(std::chrono::year{2042} / 5 / 1, std::chrono::minutes{50} + std::chrono::milliseconds{100})), "2042-05-01T00:50:00.1Z", std::partial_ordering::equivalent, true);
    basic_test<datatypes::xsd::DateTimeStamp>(ZonedTime(tz, util::construct_timepoint(std::chrono::year{2042} / 5 / 1, std::chrono::minutes{50} + std::chrono::milliseconds{123})), "2042-05-01T00:50:00.12345Z", std::partial_ordering::equivalent, true);
    basic_test<datatypes::xsd::DateTimeStamp>(ZonedTime(tz, util::construct_timepoint(std::chrono::year{2042} / 5 / 1, std::chrono::minutes{42})), "2042-05-01T00:50:00Z", std::partial_ordering::less);
    basic_test<datatypes::xsd::DateTimeStamp>(ZonedTime(Timezone{std::chrono::hours{1}}, util::construct_timepoint(std::chrono::year{2042} / 5 / 1, std::chrono::minutes{50})), "2042-05-01T00:50:00+01:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::DateTimeStamp>(ZonedTime(Timezone{std::chrono::minutes{-65}}, util::construct_timepoint(std::chrono::year{2042} / 5 / 1, std::chrono::minutes{50})), "2042-05-01T00:50:00-01:05", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::DateTimeStamp>("2042-05-05T13:40:08Z", "2042-05-05T13:40:08Z", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::DateTimeStamp>("2041-05-05T13:40:08Z", "2042-05-05T13:40:08Z", std::partial_ordering::less);
    basic_test<datatypes::xsd::DateTimeStamp>("2042-05-05T13:40:08Z", "2041-05-05T13:40:08Z", std::partial_ordering::greater);
    basic_test<datatypes::xsd::DateTimeStamp>("2042-04-05T13:40:08Z", "2042-05-05T13:40:08Z", std::partial_ordering::less);
    basic_test<datatypes::xsd::DateTimeStamp>("2042-05-05T13:40:08Z", "2042-04-05T13:40:08Z", std::partial_ordering::greater);
    basic_test<datatypes::xsd::DateTimeStamp>("2042-05-04T13:40:08Z", "2042-05-05T13:40:08Z", std::partial_ordering::less);
    basic_test<datatypes::xsd::DateTimeStamp>("2042-05-05T13:40:08Z", "2042-05-04T13:40:08Z", std::partial_ordering::greater);
    basic_test<datatypes::xsd::DateTimeStamp>("2042-05-05T12:40:08Z", "2042-05-05T13:40:08Z", std::partial_ordering::less);
    basic_test<datatypes::xsd::DateTimeStamp>("2042-05-05T13:40:08Z", "2042-05-05T12:40:08Z", std::partial_ordering::greater);
    basic_test<datatypes::xsd::DateTimeStamp>("2042-05-05T13:39:08Z", "2042-05-05T13:40:08Z", std::partial_ordering::less);
    basic_test<datatypes::xsd::DateTimeStamp>("2042-05-05T13:40:08Z", "2042-05-05T13:39:08Z", std::partial_ordering::greater);
    basic_test<datatypes::xsd::DateTimeStamp>("2042-05-05T13:40:07Z", "2042-05-05T13:40:08Z", std::partial_ordering::less);
    basic_test<datatypes::xsd::DateTimeStamp>("2042-05-05T13:40:08Z", "2042-05-05T13:40:07Z", std::partial_ordering::greater);
    basic_test<datatypes::xsd::DateTimeStamp>("2042-05-05T13:40:08.005Z", "2042-05-05T13:40:08.006Z", std::partial_ordering::less);
    basic_test<datatypes::xsd::DateTimeStamp>("2042-05-05T13:40:08.006Z", "2042-05-05T13:40:08.005Z", std::partial_ordering::greater);
    basic_test<datatypes::xsd::DateTimeStamp>("2042-05-05T13:40:08Z", "2042-05-05T14:40:08+01:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::DateTimeStamp>("2042-05-05T13:40:08Z", "2042-05-05T12:30:08-01:10", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::DateTimeStamp>(ZonedTime(Timezone{std::chrono::hours{-14}}, static_cast<std::chrono::local_days>(std::chrono::year::min() / 1 / 1) + std::chrono::hours{0}), "-32767-01-01T00:00:00-14:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::DateTimeStamp>(ZonedTime(Timezone{std::chrono::hours{14}}, static_cast<std::chrono::local_days>(std::chrono::year::max() / 12 / 31) + (std::chrono::hours{24} - std::chrono::milliseconds{1})), "32767-12-31T23:59:59.999+14:00", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::DateTimeStamp>(ZonedTime(tz, static_cast<std::chrono::local_days>(std::chrono::year::min() / 1 / 1) + std::chrono::hours{0}), "-32767-01-01T00:00:00Z", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::DateTimeStamp>(ZonedTime(tz, static_cast<std::chrono::local_days>(std::chrono::year::max() / 12 / 31) + (std::chrono::hours{24} - std::chrono::milliseconds{1})), "32767-12-31T23:59:59.999Z", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::DateTimeStamp>(ZonedTime(tz, static_cast<std::chrono::local_days>(std::chrono::year::max() / 12 / 31) + (std::chrono::hours{24} - std::chrono::seconds{1})), "32767-12-31T23:59:59Z", std::partial_ordering::equivalent);
    CHECK(Literal::make_typed<datatypes::xsd::DateTimeStamp>("-32767-01-01T00:00:00.000Z").is_inlined());
    CHECK(Literal::make_typed<datatypes::xsd::DateTimeStamp>("32767-12-31T23:59:59.000Z").is_inlined());
    CHECK(!Literal::make_typed<datatypes::xsd::DateTimeStamp>("-32767-01-01T00:00:00.000-14:00").is_inlined());
    CHECK(!Literal::make_typed<datatypes::xsd::DateTimeStamp>("32767-12-31T23:59:59.999+14:00").is_inlined());
    CHECK(storage::default_node_storage.has_specialized_storage_for(datatypes::xsd::DateTimeStamp::fixed_id));
    Literal a{};
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-05-05T-10:00:00.000Z"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-05-05T25:00:00.000Z"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-05-05T00:-1:00.000Z"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-05-05T00:70:00.000Z"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-05-05T00:00:-1.000Z"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-05-05T00:00:70.000Z"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-05-05T00:00:5.-100Z"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-01-00T00:00:00.000Z"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-00-01T00:00:00.000Z"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-12-32T00:00:00.000Z"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-13-30T00:00:00.000Z"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-02-30T00:00:00.000Z"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-02-24T00:00:00.000"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-02-24T00:00:00.000+20:00"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-02-24T00:00:00.000-20:00"), std::runtime_error);
    CHECK(a == Literal{});  // turn off unused and nodiscard ignored warnings
    CHECK(Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-05-06T00:00:00.000Z") == Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-05-05T24:00:00Z"));
    CHECK(Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-05-05T24:00:00.000Z").lexical_form() == "2042-05-06T00:00:00Z");
}

TEST_CASE("datatype duration") {
    using namespace rdf4cpp;

    CHECK(std::string(datatypes::xsd::Duration::identifier) == "http://www.w3.org/2001/XMLSchema#duration");

    basic_test<datatypes::xsd::Duration>(std::make_pair(std::chrono::months{0}, std::chrono::minutes{0}), "PT0S", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::Duration>(std::make_pair(std::chrono::months{0}, std::chrono::minutes{1}), "PT1M", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::Duration>(std::make_pair(std::chrono::months{0}, std::chrono::seconds{1}), "PT01S", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::Duration>(std::make_pair(std::chrono::months{0}, std::chrono::hours{1}), "PT1H", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::Duration>(std::make_pair(std::chrono::months{0}, std::chrono::days{1}), "P1D", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::Duration>(std::make_pair(std::chrono::months{1}, std::chrono::minutes{0}), "P1M", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::Duration>(std::make_pair(std::chrono::years{1}, std::chrono::minutes{0}), "P1Y", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::Duration>(std::make_pair(std::chrono::months{1}, std::chrono::minutes{1}), "P1MT1M", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::Duration>(std::make_pair(std::chrono::months{-1}, std::chrono::minutes{-1}), "-P1MT1M", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::Duration>(std::make_pair(std::chrono::months{14}, std::chrono::days{3}+std::chrono::hours{4}+std::chrono::minutes{5}+std::chrono::seconds{6}), "P1Y2M3DT4H5M06S", std::partial_ordering::equivalent);
    CHECK(Literal::make_typed<datatypes::xsd::Duration>("PT1M").is_inlined());
    CHECK(Literal::make_typed<datatypes::xsd::Duration>("P1M").is_inlined());
    CHECK(Literal::make_typed<datatypes::xsd::Duration>("P365D").is_inlined());
    CHECK(!Literal::make_typed<datatypes::xsd::Duration>("P5000D").is_inlined());
    CHECK(storage::default_node_storage.has_specialized_storage_for(datatypes::xsd::Duration::fixed_id));

    Literal a{};
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::Duration>(""), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::Duration>("P"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::Duration>("PT"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::Duration>("P5M24Y"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::Duration>("P5YABC"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::Duration>("-P5Y-3D"), std::runtime_error);
    CHECK(a == Literal{}); // turn off unused and nodiscard ignored warnings

    basic_test<datatypes::xsd::Duration>("P1M", "P30D", std::partial_ordering::unordered);
    basic_test<datatypes::xsd::Duration>("P1M", "P2M", std::partial_ordering::less);
    basic_test<datatypes::xsd::Duration>("PT1M", "PT30S", std::partial_ordering::greater);
    basic_test<datatypes::xsd::Duration>("PT1M", "PT2M", std::partial_ordering::less);
}

TEST_CASE("datatype dayTimeDuration") {
    using namespace rdf4cpp;

    CHECK(std::string(datatypes::xsd::DayTimeDuration::identifier) == "http://www.w3.org/2001/XMLSchema#dayTimeDuration");

    basic_test<datatypes::xsd::DayTimeDuration>(std::chrono::minutes{0}, "PT0S", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::DayTimeDuration>(std::chrono::minutes{1}, "PT1M", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::DayTimeDuration>(std::chrono::seconds{1}, "PT01S", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::DayTimeDuration>(std::chrono::hours{1}, "PT1H", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::DayTimeDuration>(std::chrono::days{1}, "P1D", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::DayTimeDuration>(std::chrono::days{1}+std::chrono::hours{2}+std::chrono::minutes{3}+std::chrono::seconds{4}, "P1DT2H3M04S", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::DayTimeDuration>(std::chrono::days{-1}, "-P1D", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::DayTimeDuration>(std::chrono::milliseconds::max(), "P106751991167DT7H12M55.807S", std::partial_ordering::equivalent);
    CHECK(Literal::make_typed<datatypes::xsd::DayTimeDuration>("P500DT42M").is_inlined());
    CHECK(storage::default_node_storage.has_specialized_storage_for(datatypes::xsd::DayTimeDuration::fixed_id));
    Literal a{};
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DayTimeDuration>(""), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DayTimeDuration>("P"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DayTimeDuration>("PT"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DayTimeDuration>("PT5M24H"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DayTimeDuration>("P5DABC"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DayTimeDuration>("P10Y"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::DayTimeDuration>("P5M"), std::runtime_error);
    CHECK(a == Literal{}); // turn off unused and nodiscard ignored warnings

    basic_test<datatypes::xsd::DayTimeDuration>("PT1M", "PT30S", std::partial_ordering::greater);
    basic_test<datatypes::xsd::DayTimeDuration>("PT1M", "PT2M", std::partial_ordering::less);
}

TEST_CASE("datatype yearMonthDuration") {
    using namespace rdf4cpp;

    CHECK(std::string(datatypes::xsd::YearMonthDuration::identifier) == "http://www.w3.org/2001/XMLSchema#yearMonthDuration");

    basic_test<datatypes::xsd::YearMonthDuration>(std::chrono::months{0}, "P0M", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::YearMonthDuration>(std::chrono::years{1}, "P1Y", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::YearMonthDuration>(std::chrono::years{1} + std::chrono::months{1}, "P1Y1M", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::YearMonthDuration>(std::chrono::years{-1} + std::chrono::months{-1}, "-P1Y1M", std::partial_ordering::equivalent);
    basic_test<datatypes::xsd::YearMonthDuration>(std::chrono::months::max(), "P768614336404564650Y7M", std::partial_ordering::equivalent);
    CHECK(Literal::make_typed<datatypes::xsd::YearMonthDuration>("P500Y30M").is_inlined());
    CHECK(storage::default_node_storage.has_specialized_storage_for(datatypes::xsd::YearMonthDuration::fixed_id));
    Literal a{};
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::YearMonthDuration>(""), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::YearMonthDuration>("P"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::YearMonthDuration>("PT"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::YearMonthDuration>("P5M24Y"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::YearMonthDuration>("P5YABC"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::YearMonthDuration>("PT10H"), std::runtime_error);
    CHECK_THROWS_AS(a = Literal::make_typed<datatypes::xsd::YearMonthDuration>("P5D"), std::runtime_error);
    CHECK(a == Literal{}); // turn off unused and nodiscard ignored warnings

    basic_test<datatypes::xsd::YearMonthDuration>("P1M", "P2M", std::partial_ordering::less);
    basic_test<datatypes::xsd::YearMonthDuration>("P1Y", "P1M", std::partial_ordering::greater);
}

TEST_CASE("cross compare") {
    using namespace rdf4cpp;

    CHECK(Literal::make_typed<datatypes::xsd::Date>("2042-5-5") < Literal::make_typed<datatypes::xsd::DateTime>("2042-5-5T10:0:0"));
    CHECK(Literal::make_typed<datatypes::xsd::Date>("2042-5-5") > Literal::make_typed<datatypes::xsd::GDay>("---5"));
    CHECK(Literal::make_typed<datatypes::xsd::GDay>("---5") < Literal::make_typed<datatypes::xsd::DateTime>("2042-5-5T10:0:0"));
    CHECK(Literal::make_typed<datatypes::xsd::GMonth>("--5") < Literal::make_typed<datatypes::xsd::DateTime>("2042-5-5T10:0:0"));
    CHECK(Literal::make_typed<datatypes::xsd::GYear>("2043") > Literal::make_typed<datatypes::xsd::DateTime>("2042-5-5T10:0:0"));
    CHECK(Literal::make_typed<datatypes::xsd::GYearMonth>("2043-5") > Literal::make_typed<datatypes::xsd::DateTime>("2042-5-5T10:0:0"));
    CHECK(Literal::make_typed<datatypes::xsd::GMonthDay>("--5-5") < Literal::make_typed<datatypes::xsd::DateTime>("2042-5-5T10:0:0"));
    CHECK(Literal::make_typed<datatypes::xsd::Time>("12:0:0") > Literal::make_typed<datatypes::xsd::DateTime>("1972-12-31T10:0:0"));
    CHECK(Literal::make_typed<datatypes::xsd::DateTimeStamp>("1972-12-31T12:0:0Z") > Literal::make_typed<datatypes::xsd::DateTime>("1972-12-31T10:0:0Z"));

    CHECK(Literal::make_typed<datatypes::xsd::YearMonthDuration>("P1Y") < Literal::make_typed<datatypes::xsd::Duration>("P1YT1H"));
    CHECK(Literal::make_typed<datatypes::xsd::DayTimeDuration>("PT1H") < Literal::make_typed<datatypes::xsd::Duration>("P1YT1H"));
}

TEST_CASE("Literal API") {
    using namespace rdf4cpp;

    CHECK(Literal::now().datatype_eq<datatypes::xsd::DateTime>());

    CHECK(Literal::make_typed<datatypes::xsd::Date>("2042-5-6").as_year() == Literal::make_typed<datatypes::xsd::Integer>("2042"));
    CHECK(Literal::make_typed<datatypes::xsd::DateTime>("2042-5-6T1:2:3").as_year() == Literal::make_typed<datatypes::xsd::Integer>("2042"));
    CHECK(Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-5-6T1:2:3Z").as_year() == Literal::make_typed<datatypes::xsd::Integer>("2042"));
    CHECK(Literal::make_typed<datatypes::xsd::GYear>("2042").as_year() == Literal::make_typed<datatypes::xsd::Integer>("2042"));
    CHECK(Literal::make_typed<datatypes::xsd::GYearMonth>("2042-5").as_year() == Literal::make_typed<datatypes::xsd::Integer>("2042"));
    CHECK(Literal::make_typed<datatypes::xsd::GDay>("---5").as_year() == Literal{});
    CHECK(Literal::make_simple("5").as_year() == Literal{});

    CHECK(Literal::make_typed<datatypes::xsd::Date>("2042-5-6").as_month() == Literal::make_typed<datatypes::xsd::Integer>("5"));
    CHECK(Literal::make_typed<datatypes::xsd::DateTime>("2042-5-6T1:2:3").as_month() == Literal::make_typed<datatypes::xsd::Integer>("5"));
    CHECK(Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-5-6T1:2:3Z").as_month() == Literal::make_typed<datatypes::xsd::Integer>("5"));
    CHECK(Literal::make_typed<datatypes::xsd::GMonth>("--5").as_month() == Literal::make_typed<datatypes::xsd::Integer>("5"));
    CHECK(Literal::make_typed<datatypes::xsd::GMonthDay>("--5-6").as_month() == Literal::make_typed<datatypes::xsd::Integer>("5"));
    CHECK(Literal::make_typed<datatypes::xsd::GYearMonth>("2042-5").as_month() == Literal::make_typed<datatypes::xsd::Integer>("5"));
    CHECK(Literal::make_typed<datatypes::xsd::GDay>("---5").as_month() == Literal{});
    CHECK(Literal::make_simple("5").as_month() == Literal{});

    CHECK(Literal::make_typed<datatypes::xsd::Date>("2042-5-6").as_day() == Literal::make_typed<datatypes::xsd::Integer>("6"));
    CHECK(Literal::make_typed<datatypes::xsd::DateTime>("2042-5-6T1:2:3").as_day() == Literal::make_typed<datatypes::xsd::Integer>("6"));
    CHECK(Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-5-6T1:2:3Z").as_day() == Literal::make_typed<datatypes::xsd::Integer>("6"));
    CHECK(Literal::make_typed<datatypes::xsd::GDay>("---5").as_day() == Literal::make_typed<datatypes::xsd::Integer>("5"));
    CHECK(Literal::make_typed<datatypes::xsd::GMonthDay>("--5-6").as_day() == Literal::make_typed<datatypes::xsd::Integer>("6"));
    CHECK(Literal::make_typed<datatypes::xsd::GMonth>("--5").as_day() == Literal{});
    CHECK(Literal::make_simple("5").as_day() == Literal{});

    CHECK(Literal::make_typed<datatypes::xsd::Time>("1:2:3").as_hours() == Literal::make_typed<datatypes::xsd::Integer>("1"));
    CHECK(Literal::make_typed<datatypes::xsd::DateTime>("2042-5-6T1:2:3+1:0").as_hours() == Literal::make_typed<datatypes::xsd::Integer>("1"));
    CHECK(Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-5-6T1:2:3+1:0").as_hours() == Literal::make_typed<datatypes::xsd::Integer>("1"));
    CHECK(Literal::make_typed<datatypes::xsd::GMonth>("--5").as_hours() == Literal{});
    CHECK(Literal::make_simple("5").as_hours() == Literal{});

    CHECK(Literal::make_typed<datatypes::xsd::Time>("1:2:3").as_minutes() == Literal::make_typed<datatypes::xsd::Integer>("2"));
    CHECK(Literal::make_typed<datatypes::xsd::DateTime>("2042-5-6T1:2:3+1:0").as_minutes() == Literal::make_typed<datatypes::xsd::Integer>("2"));
    CHECK(Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-5-6T1:2:3+1:0").as_minutes() == Literal::make_typed<datatypes::xsd::Integer>("2"));
    CHECK(Literal::make_typed<datatypes::xsd::GMonth>("--5").as_minutes() == Literal{});
    CHECK(Literal::make_simple("5").as_minutes() == Literal{});

    CHECK(Literal::make_typed<datatypes::xsd::Time>("1:2:3").as_seconds() == Literal::make_typed<datatypes::xsd::Decimal>("3"));
    CHECK(Literal::make_typed<datatypes::xsd::DateTime>("2042-5-6T1:2:3+1:0").as_seconds() == Literal::make_typed<datatypes::xsd::Decimal>("3"));
    CHECK(Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-5-6T1:2:3+1:0").as_seconds() == Literal::make_typed<datatypes::xsd::Decimal>("3"));
    CHECK(Literal::make_typed<datatypes::xsd::Time>("1:2:59.999").as_seconds() == Literal::make_typed<datatypes::xsd::Decimal>("59.999"));
    CHECK(Literal::make_typed<datatypes::xsd::GMonth>("--5").as_seconds() == Literal{});
    CHECK(Literal::make_simple("5").as_seconds() == Literal{});

    CHECK(Literal::make_typed<datatypes::xsd::DateTime>("2042-5-6T1:2:3+1:0").as_timezone() == Literal::make_typed<datatypes::xsd::DayTimeDuration>("PT1H"));
    CHECK(Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-5-6T1:2:3Z").as_timezone() == Literal::make_typed<datatypes::xsd::DayTimeDuration>("PT0H"));
    CHECK(Literal::make_typed<datatypes::xsd::GDay>("---3+1:30").as_timezone() == Literal::make_typed<datatypes::xsd::DayTimeDuration>("PT1H30M"));
    CHECK(Literal::make_typed<datatypes::xsd::GDay>("---3").as_timezone() == Literal{});
    CHECK(Literal::make_simple("5").as_timezone() == Literal{});

    CHECK(Literal::make_typed<datatypes::xsd::DateTime>("2042-5-6T1:2:3+1:0").as_tz() == Literal::make_simple("+01:00"));
    CHECK(Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-5-6T1:2:3Z").as_tz() == Literal::make_simple("Z"));
    CHECK(Literal::make_typed<datatypes::xsd::GDay>("---3+1:30").as_tz() == Literal::make_simple("+01:30"));
    CHECK(Literal::make_typed<datatypes::xsd::GDay>("---3").as_tz() == Literal::make_simple(""));
    CHECK(Literal::make_simple("5").as_tz() == Literal{});
}

TEST_CASE("arithmetic") {
    using namespace rdf4cpp;

    CHECK(Literal::make_typed<datatypes::xsd::Date>("2042-5-6").cast_to_supertype_value<datatypes::xsd::DateTime>().has_value());
    CHECK(!Literal::make_typed<datatypes::xsd::DateTime>("2042-5-6T1:2:3").cast_to_supertype_value<datatypes::xsd::Date>().has_value());

    CHECK((Literal::make_typed<datatypes::xsd::Date>("2042-5-6") - Literal::make_typed<datatypes::xsd::Date>("2042-5-5")) == Literal::make_typed<datatypes::xsd::DayTimeDuration>("P1D"));
    CHECK((Literal::make_typed<datatypes::xsd::Date>("2042-4-6") - Literal::make_typed<datatypes::xsd::Date>("2042-5-7")) == Literal::make_typed<datatypes::xsd::DayTimeDuration>("-P31D"));
    CHECK((Literal::make_typed<datatypes::xsd::Time>("1:2:3") - Literal::make_typed<datatypes::xsd::Time>("1:1:3")) == Literal::make_typed<datatypes::xsd::DayTimeDuration>("PT1M"));
    CHECK((Literal::make_typed<datatypes::xsd::Time>("1:2:3") - Literal::make_typed<datatypes::xsd::Time>("1:3:3")) == Literal::make_typed<datatypes::xsd::DayTimeDuration>("-PT1M"));
    CHECK((Literal::make_typed<datatypes::xsd::DateTime>("2042-5-6T1:2:3") - Literal::make_typed<datatypes::xsd::DateTime>("2042-5-5T1:2:3")) == Literal::make_typed<datatypes::xsd::DayTimeDuration>("P1D"));
    CHECK((Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-5-6T1:2:3Z") - Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-5-7T1:2:3Z")) == Literal::make_typed<datatypes::xsd::DayTimeDuration>("-P1D"));
    CHECK((Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-5-6T1:2:3Z") - Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-5-6T1:2:3+10:00")) == Literal::make_typed<datatypes::xsd::DayTimeDuration>("PT10H"));
    CHECK((Literal::make_typed_from_value<datatypes::xsd::DateTime>(std::make_pair(rdf4cpp::TimePoint{std::chrono::milliseconds{std::numeric_limits<int64_t>::min()}}, std::nullopt)) - Literal::make_typed<datatypes::xsd::DateTime>("2042-5-5T1:2:3")) == Literal{});

    CHECK((Literal::make_typed<datatypes::xsd::DateTime>("2042-5-6T1:2:3") + Literal::make_typed<datatypes::xsd::DayTimeDuration>("P1DT1S")) == Literal::make_typed<datatypes::xsd::DateTime>("2042-5-7T1:2:4"));
    CHECK((Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-5-6T1:2:3Z") + Literal::make_typed<datatypes::xsd::DayTimeDuration>("-P1DT1S")) == Literal::make_typed<datatypes::xsd::DateTime>("2042-5-5T1:2:2Z"));
    CHECK((Literal::make_typed<datatypes::xsd::DateTime>("2042-5-6T1:2:3") + Literal::make_typed<datatypes::xsd::YearMonthDuration>("P1Y2M")) == Literal::make_typed<datatypes::xsd::DateTime>("2043-7-6T1:2:3"));
    CHECK((Literal::make_typed<datatypes::xsd::DateTime>("2042-5-6T1:2:3") + Literal::make_typed<datatypes::xsd::Duration>("P1Y2MT5S")) == Literal::make_typed<datatypes::xsd::DateTime>("2043-7-6T1:2:8"));
    CHECK((Literal::make_typed<datatypes::xsd::DateTime>("2042-10-6T1:2:3") + Literal::make_typed<datatypes::xsd::Duration>("P4M")) == Literal::make_typed<datatypes::xsd::DateTime>("2043-2-6T1:2:3"));
    CHECK((Literal::make_typed<datatypes::xsd::DateTime>("2042-2-6T1:2:3") + Literal::make_typed<datatypes::xsd::Duration>("-P4M")) == Literal::make_typed<datatypes::xsd::DateTime>("2041-10-6T1:2:3"));
    CHECK((Literal::make_typed<datatypes::xsd::DateTime>("2042-5-6T1:2:3") + Literal::make_typed<datatypes::xsd::Duration>("P1Y14MT5S")) == Literal::make_typed<datatypes::xsd::DateTime>("2044-7-6T1:2:8"));
    CHECK((Literal::make_typed<datatypes::xsd::Date>("2042-5-6") + Literal::make_typed<datatypes::xsd::YearMonthDuration>("P1Y")) == Literal::make_typed<datatypes::xsd::Date>("2043-5-6"));
    CHECK((Literal::make_typed<datatypes::xsd::Time>("1:2:3") + Literal::make_typed<datatypes::xsd::DayTimeDuration>("P1DT1S")) == Literal::make_typed<datatypes::xsd::Time>("1:2:4"));
    CHECK((Literal::make_typed<datatypes::xsd::DateTime>("2042-5-6T1:2:3") + Literal::make_typed_from_value<datatypes::xsd::YearMonthDuration>(std::chrono::months{std::numeric_limits<int64_t>::max()})) == Literal{});
    CHECK((Literal::make_typed<datatypes::xsd::DateTime>("2042-5-6T1:2:3") + Literal::make_typed_from_value<datatypes::xsd::DayTimeDuration>(std::chrono::milliseconds{std::numeric_limits<int64_t>::max()})) == Literal{});

    CHECK((Literal::make_typed<datatypes::xsd::DateTime>("2042-5-6T1:2:3") - Literal::make_typed<datatypes::xsd::DayTimeDuration>("P1DT1S")) == Literal::make_typed<datatypes::xsd::DateTime>("2042-5-5T1:2:2"));
    CHECK((Literal::make_typed<datatypes::xsd::DateTimeStamp>("2042-5-6T1:2:3Z") - Literal::make_typed<datatypes::xsd::DayTimeDuration>("-P1DT1S")) == Literal::make_typed<datatypes::xsd::DateTime>("2042-5-7T1:2:4Z"));
    CHECK((Literal::make_typed<datatypes::xsd::DateTime>("2042-5-6T1:2:3") - Literal::make_typed<datatypes::xsd::YearMonthDuration>("P1Y2M")) == Literal::make_typed<datatypes::xsd::DateTime>("2041-3-6T1:2:3"));
    CHECK((Literal::make_typed<datatypes::xsd::DateTime>("2042-5-6T1:2:3") - Literal::make_typed<datatypes::xsd::Duration>("P1Y2MT3S")) == Literal::make_typed<datatypes::xsd::DateTime>("2041-3-6T1:2:0"));
    CHECK((Literal::make_typed<datatypes::xsd::Date>("2042-5-6") - Literal::make_typed<datatypes::xsd::YearMonthDuration>("P1Y")) == Literal::make_typed<datatypes::xsd::Date>("2041-5-6"));
    CHECK((Literal::make_typed<datatypes::xsd::Time>("1:2:3") - Literal::make_typed<datatypes::xsd::DayTimeDuration>("P1DT1S")) == Literal::make_typed<datatypes::xsd::Time>("1:2:2"));
    CHECK((Literal::make_typed<datatypes::xsd::DateTime>("-2042-5-6T1:2:3") - Literal::make_typed_from_value<datatypes::xsd::YearMonthDuration>(std::chrono::months{std::numeric_limits<int64_t>::max()})) == Literal{});
    CHECK((Literal::make_typed<datatypes::xsd::DateTime>("-2042-5-6T1:2:3") - Literal::make_typed_from_value<datatypes::xsd::DayTimeDuration>(std::chrono::milliseconds{std::numeric_limits<int64_t>::max()})) == Literal{});

    CHECK((Literal::make_typed<datatypes::xsd::DayTimeDuration>("P1DT1S") + Literal::make_typed<datatypes::xsd::DayTimeDuration>("P1DT1H")) == Literal::make_typed<datatypes::xsd::DayTimeDuration>("P2DT1H1S"));
    CHECK((Literal::make_typed<datatypes::xsd::YearMonthDuration>("P1Y") + Literal::make_typed<datatypes::xsd::YearMonthDuration>("P1Y2M")) == Literal::make_typed<datatypes::xsd::YearMonthDuration>("P2Y2M"));
    CHECK((Literal::make_typed_from_value<datatypes::xsd::DayTimeDuration>(std::chrono::milliseconds{std::numeric_limits<int64_t>::max()}) + Literal::make_typed<datatypes::xsd::DayTimeDuration>("P1DT1H")) == Literal{});
    CHECK((Literal::make_typed_from_value<datatypes::xsd::YearMonthDuration>(std::chrono::months{std::numeric_limits<int64_t>::max()}) + Literal::make_typed<datatypes::xsd::YearMonthDuration>("P1Y2M")) == Literal{});
    CHECK((Literal::make_typed<datatypes::xsd::DayTimeDuration>("P1DT1M") - Literal::make_typed<datatypes::xsd::DayTimeDuration>("P1DT1S")) == Literal::make_typed<datatypes::xsd::DayTimeDuration>("PT59S"));
    CHECK((Literal::make_typed<datatypes::xsd::YearMonthDuration>("P1Y") - Literal::make_typed<datatypes::xsd::YearMonthDuration>("P2Y")) == Literal::make_typed<datatypes::xsd::YearMonthDuration>("-P1Y"));

    CHECK((Literal::make_typed<datatypes::xsd::DayTimeDuration>("P1DT1S") * Literal::make_typed<datatypes::xsd::Double>("2")) == Literal::make_typed<datatypes::xsd::DayTimeDuration>("P2DT2S"));
    CHECK((Literal::make_typed<datatypes::xsd::YearMonthDuration>("P1Y") * Literal::make_typed<datatypes::xsd::Double>("2")) == Literal::make_typed<datatypes::xsd::YearMonthDuration>("P2Y"));
    CHECK((Literal::make_typed<datatypes::xsd::DayTimeDuration>("P1DT1S") * Literal::make_typed_from_value<datatypes::xsd::Double>(std::numeric_limits<double>::infinity())) == Literal{});
    CHECK((Literal::make_typed<datatypes::xsd::YearMonthDuration>("P1Y") * Literal::make_typed_from_value<datatypes::xsd::Double>(std::numeric_limits<double>::infinity())) == Literal{});
    CHECK((Literal::make_typed<datatypes::xsd::DayTimeDuration>("P2DT2S") / Literal::make_typed<datatypes::xsd::Double>("2")) == Literal::make_typed<datatypes::xsd::DayTimeDuration>("P1DT1S"));
    CHECK((Literal::make_typed<datatypes::xsd::YearMonthDuration>("P2Y") / Literal::make_typed<datatypes::xsd::Double>("2")) == Literal::make_typed<datatypes::xsd::YearMonthDuration>("P1Y"));
    CHECK((Literal::make_typed<datatypes::xsd::DayTimeDuration>("P2DT2S") / Literal::make_typed_from_value<datatypes::xsd::Double>(0.0)) == Literal{});
    CHECK((Literal::make_typed<datatypes::xsd::YearMonthDuration>("P2Y") / Literal::make_typed_from_value<datatypes::xsd::Double>(0.0)) == Literal{});
    CHECK((Literal::make_typed<datatypes::xsd::DayTimeDuration>("P4D") / Literal::make_typed<datatypes::xsd::DayTimeDuration>("P2D")) == Literal::make_typed<datatypes::xsd::Decimal>("2"));
    CHECK((Literal::make_typed<datatypes::xsd::YearMonthDuration>("P4Y") / Literal::make_typed<datatypes::xsd::YearMonthDuration>("P2Y")) == Literal::make_typed<datatypes::xsd::Decimal>("2"));
}

bool baseline(rdf4cpp::datatypes::xsd::Date::cpp_type const &value, rdf4cpp::writer::BufWriterParts writer) noexcept {
    auto str = std::format("{:%Y-%m-%d}", value.first);
    if (value.second.has_value())
        str += value.second->to_canonical_string();

    return rdf4cpp::writer::write_str(str, writer);
}

bool buffered(rdf4cpp::datatypes::xsd::Date::cpp_type const &value, rdf4cpp::writer::BufWriterParts writer) noexcept {
    std::array<char, 1024> buff;
    auto str = std::format_to(&buff[0], "{:%Y-%m-%d}", value.first);
    if (value.second.has_value())
        str = value.second->to_canonical_string(str);

    return rdf4cpp::writer::write_str(std::string_view(&buff[0], str-&buff[0]), writer);
}

bool direct_write(rdf4cpp::datatypes::xsd::Date::cpp_type const &value, rdf4cpp::writer::BufWriterParts writer) noexcept {
    if (!std::format_to(rdf4cpp::writer::BufWriterOutputIterator{writer}, "{:%Y-%m-%d}", value.first).write_ok) {
        return false;
    }
    if (value.second.has_value()) {
        return value.second->to_canonical_string(writer);
    }
    return true;
}

TEST_CASE("serialization perf") {
    static constexpr size_t s = 1024;
    std::array<rdf4cpp::datatypes::xsd::Date::cpp_type, s> data{};
    std::mt19937_64 rng{};
    std::uniform_int_distribution<int> year(-5000, 5000);
    std::uniform_int_distribution<unsigned int> month(1, 12);
    std::uniform_int_distribution<unsigned int> day(1, 28);
    std::uniform_int_distribution<int> tz(-13, 13);
    std::uniform_int_distribution<int> has_tz(0, 100);
    for (auto& d : data) {
        d.first = std::chrono::year{year(rng)} / std::chrono::month{month(rng)} / day(rng);
        if (has_tz(rng) < 50)
            d.second = rdf4cpp::Timezone(std::chrono::hours(tz(rng)));
        else
            d.second = std::nullopt;
        CHECK(rdf4cpp::writer::StringWriter::oneshot([&](auto w) {
            return baseline(d, w);
        }) == rdf4cpp::writer::StringWriter::oneshot([&](auto w) {
                  return buffered(d, w);
              }));
    }

    size_t base_i = 0;
    size_t buff_i = 0;
    size_t direct_i = 0;

    ankerl::nanobench::Bench().performanceCounters(true).relative(true).run("baseline", [&]() {
        std::string r{};
        r.reserve(1024);
        rdf4cpp::writer::StringWriter wr(r);
        baseline(data[base_i % s], wr);
        ++base_i;
        wr.finalize();
        ankerl::nanobench::doNotOptimizeAway(r);
    }).run("buffered", [&]() {
        std::string r{};
        r.reserve(1024);
        rdf4cpp::writer::StringWriter wr(r);
        buffered(data[buff_i % s], wr);
        ++buff_i;
        wr.finalize();
        ankerl::nanobench::doNotOptimizeAway(r);
    }).run("direct write", [&]() {
        std::string r{};
        r.reserve(1024);
        rdf4cpp::writer::StringWriter wr(r);
        direct_write(data[direct_i % s], wr);
        ++direct_i;
        wr.finalize();
        ankerl::nanobench::doNotOptimizeAway(r);
    });
}
