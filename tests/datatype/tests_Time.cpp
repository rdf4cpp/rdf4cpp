#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("Datatype Time") {

    constexpr auto correct_type_iri_cstr = "http://www.w3.org/2001/XMLSchema#time";

    CHECK(std::string(datatypes::xsd::Time::identifier) == correct_type_iri_cstr);

    auto type_iri = IRI(datatypes::xsd::Time::identifier);
    CHECK(type_iri.identifier() == correct_type_iri_cstr);

    using type = datatypes::xsd::Time::cpp_type;

    CHECK(std::is_same_v<type, time_t>);

    struct tm tm{};

    // Fill in values for Time
    tm.tm_sec = 1; // seconds of minutes from 0 to 60
    tm.tm_min = 21; // minutes of hour from 0 to 59
    tm.tm_hour = 10; // hours of day from 0 to 24
    tm.tm_year = 0; // year since 1900
    tm.tm_mon = 0; // month of year from 0 to 11
    tm.tm_mday = 1; //date of the month
    tm.tm_isdst = -1; // value should be set even if not used
    auto value = mktime(&tm);

    auto lit1 = Literal::make<datatypes::xsd::Time>(value);
    CHECK(lit1.value<datatypes::xsd::Time>() == value);

    // Fill in values for Time
    tm.tm_sec = 6; // seconds of minutes from 0 to 61
    tm.tm_min = 20; // minutes of hour from 0 to 59
    tm.tm_hour = 10; // hours of day from 0 to 24
    tm.tm_year = 0; // year since 1900
    tm.tm_mon = 0; // month of year from 0 to 11
    tm.tm_mday = 1; //date of the month
    tm.tm_isdst = -1; // value should be set even if not used
    value = mktime(&tm);

    auto lit2 = Literal::make<datatypes::xsd::Time>(value);
    CHECK(lit2.value<datatypes::xsd::Time>() == value);

    // Fill in values for Time
    tm.tm_sec = 61; // seconds of minutes from 0 to 60
    tm.tm_min = 20; // minutes of hour from 0 to 59
    tm.tm_hour = 10; // hours of day from 0 to 24
    tm.tm_year = 0; // year since 1900
    tm.tm_mon = 0; // month of year from 0 to 11
    tm.tm_mday = 1; //date of the month
    tm.tm_isdst = -1; // value should be set even if not used
    value = mktime(&tm);

    auto lit3 = Literal::make<datatypes::xsd::Time>(value);
    CHECK(lit3.value<datatypes::xsd::Time>() == value);

    // Fill in values for Time
    tm.tm_sec = 120; // seconds of minutes from 0 to 60
    tm.tm_min = 20; // minutes of hour from 0 to 59
    tm.tm_hour = 10; // hours of day from 0 to 24
    tm.tm_year = 0; // year since 1900
    tm.tm_mon = 0; // month of year from 0 to 11
    tm.tm_mday = 1; //date of the month
    tm.tm_isdst = -1; // value should be set even if not used
    value = mktime(&tm);

    auto lit4 = Literal::make<datatypes::xsd::Time>(value);
    CHECK(lit4.value<datatypes::xsd::Time>() == value);

    auto lit5 = Literal("10:22:00", type_iri);
    CHECK(lit5.value<datatypes::xsd::Time>() == value);

    // Fill in values for Time
    tm.tm_sec = 0; // seconds of minutes from 0 to 60
    tm.tm_min = 0; // minutes of hour from 0 to 59
    tm.tm_hour = 24; // hours of day from 0 to 24
    tm.tm_year = 0; // year since 1900
    tm.tm_mon = 0; // month of year from 0 to 11
    tm.tm_mday = 0; //date of the month to be set to 0, if tm_hour > 23
    tm.tm_isdst = -1; // value should be set even if not used
    value = mktime(&tm);

    auto lit6 = Literal::make<datatypes::xsd::Time>(value);
    CHECK(lit6.value<datatypes::xsd::Time>() == value);

    auto lit7 = Literal("00:00:00", type_iri);
    CHECK(lit7.value<datatypes::xsd::Time>() == value);

    CHECK(lit1 != lit2);
    CHECK(lit2 != lit3);
    CHECK(lit1 == lit3);
    CHECK(lit4 == lit5);
    CHECK(lit6 == lit7);

    // suppress warnings regarding attribute ‘nodiscard’
    Literal no_discard_dummy;

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("T10:04:100", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("180:04:100", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("24:00:01", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("02-00-01", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("22-04-2022", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("22-04-2022T10:10:10", type_iri), "XSD Parsing Error", std::runtime_error);
}