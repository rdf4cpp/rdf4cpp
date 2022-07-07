#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("Datatype GMonthDay") {

    constexpr auto correct_type_iri_cstr = "http://www.w3.org/2001/XMLSchema#gMonthDay";

    CHECK(std::string(datatypes::xsd::GMonthDay::identifier) == correct_type_iri_cstr);

    auto type_iri = IRI(datatypes::xsd::GMonthDay::identifier);
    CHECK(type_iri.identifier() == correct_type_iri_cstr);

    using type = datatypes::xsd::GMonthDay::cpp_type;

    CHECK(std::is_same_v<type, time_t>);

    time_t value;
    struct tm tm{};

    // Fill in values for GMonthDay
    tm.tm_sec = 0; // seconds of minutes from 0 to 61
    tm.tm_min = 0; // minutes of hour from 0 to 59
    tm.tm_hour = 0; // hours of day from 0 to 24
    tm.tm_year = 0; // year since 1900
    tm.tm_mon = 5 - 1; // month of year from 0 to 11
    tm.tm_mday = 1; // date of the month
    tm.tm_isdst = -1; // value should be set even if not used
    value = mktime(&tm);

    auto lit1 = Literal::make<datatypes::xsd::GMonthDay>(value);
    CHECK(lit1.value<datatypes::xsd::GMonthDay>() == value);

    // Fill in values for GMonthDay
    tm.tm_sec = 0; // seconds of minutes from 0 to 61
    tm.tm_min = 0; // minutes of hour from 0 to 59
    tm.tm_hour = 0; // hours of day from 0 to 24
    tm.tm_year = 0; // year since 1900
    tm.tm_mon = 5 - 1; // month of year from 0 to 11
    tm.tm_mday = 31; // date of the month
    tm.tm_isdst = -1; // value should be set even if not used
    value = mktime(&tm);

    auto lit2 = Literal::make<datatypes::xsd::GMonthDay>(value);
    CHECK(lit2.value<datatypes::xsd::GMonthDay>() == value);

    // Fill in values for GMonthDay
    tm.tm_sec = 0; // seconds of minutes from 0 to 61
    tm.tm_min = 0; // minutes of hour from 0 to 59
    tm.tm_hour = 0; // hours of day from 0 to 24
    tm.tm_year = 0; // year since 1900
    tm.tm_mon = 5 - 1; // month of year from 0 to 11
    tm.tm_mday = 1; // date of the month
    tm.tm_isdst = -1; // value should be set even if not used
    value = mktime(&tm);

    auto lit3 = Literal::make<datatypes::xsd::GMonthDay>(value);
    CHECK(lit3.value<datatypes::xsd::GMonthDay>() == value);

    CHECK(lit1 != lit2);
    CHECK(lit2 != lit3);
    CHECK(lit1 == lit3);

    // suppress warnings regarding attribute ‘nodiscard’
    Literal no_discard_dummy;

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("22-04-2022", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("22-04-2022T10:10:10", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("22:04:30", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("22-04-2022T10:10:10Z", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("abc", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("05", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("-05-22", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("--05-22\n", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("--05-22\t", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("--05-22\r", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("05-22", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("--13-22", type_iri), "XSD Parsing Error", std::runtime_error);
}