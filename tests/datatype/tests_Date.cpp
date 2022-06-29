#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("Datatype Date") {

    constexpr auto correct_type_iri_cstr = "http://www.w3.org/2001/XMLSchema#date";

    CHECK(std::string(datatypes::xsd::Date::identifier) == correct_type_iri_cstr);

    auto type_iri = IRI(datatypes::xsd::Date::identifier);
    CHECK(type_iri.identifier() == correct_type_iri_cstr);

    using type = datatypes::xsd::Date::cpp_type;

    CHECK(std::is_same_v<type, time_t>);

    struct tm tm{};

    // Fill in values for Date
    tm.tm_sec = 0; // seconds of minutes from 0 to 61
    tm.tm_min = 0; // minutes of hour from 0 to 59
    tm.tm_hour = 0; // hours of day from 0 to 24
    tm.tm_year = 2022 - 1900; // year since 1900
    tm.tm_mon = 4 - 1; // month of year from 0 to 11
    tm.tm_mday = 22;
    tm.tm_isdst = -1; // value should be set even if not used
    auto value = mktime(&tm);

    auto lit1 = Literal::make<datatypes::xsd::Date>(value);
    CHECK(lit1.value<datatypes::xsd::Date>() == value);

    // Fill in values for Date
    tm.tm_sec = 0; // seconds of minutes from 0 to 61
    tm.tm_min = 0; // minutes of hour from 0 to 59
    tm.tm_hour = 0; // hours of day from 0 to 24
    tm.tm_year = 2022 - 1900; // year since 1900
    tm.tm_mon = 5 - 1; // month of year from 0 to 11
    tm.tm_mday = 21;
    tm.tm_isdst = -1; // value should be set even if not used
    value = mktime(&tm);

    auto lit2 = Literal::make<datatypes::xsd::Date>(value);
    CHECK(lit2.value<datatypes::xsd::Date>() == value);

    // Fill in values for Date
    tm.tm_sec = 0; // seconds of minutes from 0 to 61
    tm.tm_min = 0; // minutes of hour from 0 to 59
    tm.tm_hour = 0; // hours of day from 0 to 24
    tm.tm_year = 2022 - 1900; // year since 1900
    tm.tm_mon = 4 - 1; // month of year from 0 to 11
    tm.tm_mday = 22;
    tm.tm_isdst = -1; // value should be set even if not used
    value = mktime(&tm);

    auto lit3 = Literal::make<datatypes::xsd::Date>(value);
    CHECK(lit3.value<datatypes::xsd::Date>() == value);

    // Fill in values for Date
    tm.tm_sec = 0; // seconds of minutes from 0 to 61
    tm.tm_min = 0; // minutes of hour from 0 to 59
    tm.tm_hour = 0; // hours of day from 0 to 24
    tm.tm_year = 2022 - 1900; // year since 1900
    tm.tm_mon = 2 - 1; // month of year from 0 to 11
    tm.tm_mday = 30;
    tm.tm_isdst = -1; // value should be set even if not used
    value = mktime(&tm);

    auto lit4 = Literal::make<datatypes::xsd::Date>(value);
    CHECK(lit4.value<datatypes::xsd::Date>() == value);

    auto lit5 = Literal("2022-03-02", type_iri);
    CHECK(lit5.value<datatypes::xsd::Date>() == value);

    // Fill in values for Date
    tm.tm_sec = 0; // seconds of minutes from 0 to 61
    tm.tm_min = 0; // minutes of hour from 0 to 59
    tm.tm_hour = 0; // hours of day from 0 to 24
    tm.tm_year = 2022 - 1900; // year since 1900
    tm.tm_mon = 6 - 1; // month of year from 0 to 11
    tm.tm_mday = 31;
    tm.tm_isdst = -1; // value should be set even if not used
    value = mktime(&tm);

    auto lit6 = Literal("2022-07-01", type_iri);
    CHECK(lit6.value<datatypes::xsd::Date>() == value);

    auto lit7 = Literal::make<datatypes::xsd::Date>(value);
    CHECK(lit7.value<datatypes::xsd::Date>() == value);

    CHECK(lit1 != lit2);
    CHECK(lit2 != lit3);
    CHECK(lit1 == lit3);
    CHECK(lit4 == lit5);
    CHECK(lit6 == lit7);

    // suppress warnings regarding attribute ‘nodiscard’
    Literal no_discard_dummy;

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("22-04-2022", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("2022/04/22", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("qwerty", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("2022/04-22", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("22-04-2022T10:10:10", type_iri), "XSD Parsing Error", std::runtime_error);
}