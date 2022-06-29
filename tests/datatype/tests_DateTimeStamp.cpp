#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("Datatype DateTimeStamp") {

    constexpr auto correct_type_iri_cstr = "http://www.w3.org/2001/XMLSchema#dateTimeStamp";

    CHECK(std::string(datatypes::xsd::DateTimeStamp::identifier) == correct_type_iri_cstr);

    auto type_iri = IRI(datatypes::xsd::DateTimeStamp::identifier);
    CHECK(type_iri.identifier() == correct_type_iri_cstr);

    using type = datatypes::xsd::DateTimeStamp::cpp_type;

    CHECK(std::is_same_v<type, time_t>);

    struct tm tm{};

    // Fill in values for DateTime
    tm.tm_sec = 1; // seconds of minutes from 0 to 61
    tm.tm_min = 21; // minutes of hour from 0 to 59
    tm.tm_hour = 5; // hours of day from 0 to 24
    tm.tm_year = 2022 - 1900; // year since 1900
    tm.tm_mon = 4 - 1; // month of year from 0 to 11
    tm.tm_mday = 22; // date of the month
    tm.tm_isdst = -1; // value should be set even if not used
    tm.tm_zone = "CEST"; //timezone
    auto value = mktime(&tm);

    auto lit1 = Literal::make<datatypes::xsd::DateTimeStamp>(value);
    CHECK(lit1.value<datatypes::xsd::DateTimeStamp>() == value);

    // Fill in values for DateTime
    tm.tm_sec = 1; // seconds of minutes from 0 to 61
    tm.tm_min = 21; // minutes of hour from 0 to 59
    tm.tm_hour = 10; // hours of day from 0 to 24
    tm.tm_year = 2022 - 1900; // year since 1900
    tm.tm_mon = 5 - 1; // month of year from 0 to 11
    tm.tm_mday = 21; // date of the month
    tm.tm_isdst = -1; // value should be set even if not used
    tm.tm_zone = "CEST"; //timezone
    value = mktime(&tm);

    auto lit2 = Literal::make<datatypes::xsd::DateTimeStamp>(value);
    CHECK(lit2.value<datatypes::xsd::DateTimeStamp>() == value);

    // Fill in values for DateTime
    tm.tm_sec = 61; // seconds of minutes from 0 to 61
    tm.tm_min = 20; // minutes of hour from 0 to 59
    tm.tm_hour = 5; // hours of day from 0 to 24
    tm.tm_year = 2022 - 1900; // year since 1900
    tm.tm_mon = 4 - 1; // month of year from 0 to 11
    tm.tm_mday = 22; // date of the month
    tm.tm_isdst = -1; // value should be set even if not used
    tm.tm_zone = "CEST"; //timezone
    value = mktime(&tm);

    auto lit3 = Literal::make<datatypes::xsd::DateTimeStamp>(value);
    CHECK(lit3.value<datatypes::xsd::DateTimeStamp>() == value);

    auto lit4 = Literal("2022-04-22T05:21:01Z", type_iri);
    CHECK(lit4.value<datatypes::xsd::DateTimeStamp>() == value);

    CHECK(lit1 != lit2);
    CHECK(lit2 != lit3);
    CHECK(lit1 == lit3);
    CHECK(lit1 == lit4);
    CHECK(lit3 == lit4);

    // suppress warnings regarding attribute ‘nodiscard’
    Literal no_discard_dummy;

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("T10:04:100", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("P180:04:100", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("24:00:01Z", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("22-04-20236", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("PYMDHMS", type_iri), "XSD Parsing Error", std::runtime_error);
}