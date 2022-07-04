#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("Datatype YearMonthDuration") {

    constexpr auto correct_type_iri_cstr = "http://www.w3.org/2001/XMLSchema#yearMonthDuration";

    CHECK(std::string(datatypes::xsd::YearMonthDuration::identifier) == correct_type_iri_cstr);

    auto type_iri = IRI(datatypes::xsd::YearMonthDuration::identifier);
    CHECK(type_iri.identifier() == correct_type_iri_cstr);

    using type = datatypes::xsd::YearMonthDuration::cpp_type;

    CHECK(std::is_same_v<type, time_t>);

    struct tm tm{};

    /* By default these values should be set as following for proper creation of lexical form.
     * tm.tm_mday = 0;  // day
     * tm.tm_sec = 0;   // seconds of minutes
     * tm.tm_min = 1;   // minutes of hour
     * tm.tm_hour = 0; // hours of day
     * tm.tm_isdst = -1; // daylight saving
     */

    //default values
    tm.tm_year = 1;  // year
    tm.tm_mon = 0;  // month
    tm.tm_mday = 1;  // day
    tm.tm_sec = 0;   // seconds of minutes
    tm.tm_min = 0;   // minutes of hour
    tm.tm_hour = 0; // hours of day
    tm.tm_isdst = -1; // daylight saving

    auto value = mktime(&tm);

    auto lit1 = Literal::make<datatypes::xsd::YearMonthDuration>(value);
    CHECK(lit1.value<datatypes::xsd::YearMonthDuration>() == value);

    std::cout<< lit1 <<std::endl;

    //default values
    tm.tm_year = 1;  // year
    tm.tm_mon = 40;  // month
    tm.tm_mday = 1;  // day
    tm.tm_sec = 0;   // seconds of minutes
    tm.tm_min = 0;   // minutes of hour
    tm.tm_hour = 0; // hours of day
    tm.tm_isdst = -1; // daylight saving

    value = mktime(&tm);

    auto lit2 = Literal::make<datatypes::xsd::YearMonthDuration>(value);
    CHECK(lit2.value<datatypes::xsd::YearMonthDuration>() == value);

    std::cout<< lit2 <<std::endl;

    //default values
    tm.tm_year = 0;  // year
    tm.tm_mon = 24;  // month
    tm.tm_mday = 1;  // day
    tm.tm_sec = 0;   // seconds of minutes
    tm.tm_min = 0;   // minutes of hour
    tm.tm_hour = 0; // hours of day
    tm.tm_isdst = -1; // daylight saving

    value = mktime(&tm);

    auto lit3 = Literal::make<datatypes::xsd::YearMonthDuration>(value);
    CHECK(lit3.value<datatypes::xsd::YearMonthDuration>() == value);

    std::cout<< lit3 <<std::endl;

    //default values
    tm.tm_year = 1000;  // year
    tm.tm_mon = 0;  // month
    tm.tm_mday = 1;  // day
    tm.tm_sec = 0;   // seconds of minutes
    tm.tm_min = 0;   // minutes of hour
    tm.tm_hour = 0; // hours of day
    tm.tm_isdst = -1; // daylight saving

    value = mktime(&tm);

    auto lit4 = Literal::make<datatypes::xsd::YearMonthDuration>(value);
    CHECK(lit4.value<datatypes::xsd::YearMonthDuration>() == value);

    std::cout<< lit4 <<std::endl;

    //default values
    tm.tm_year = 1;  // year
    tm.tm_mon = 1;  // month
    tm.tm_mday = 1;  // day
    tm.tm_sec = 0;   // seconds of minutes
    tm.tm_min = 0;   // minutes of hour
    tm.tm_hour = 0; // hours of day
    tm.tm_isdst = 0; // daylight saving

    value = mktime(&tm);

    auto lit5 = Literal::make<datatypes::xsd::YearMonthDuration>(value);
    CHECK(lit5.value<datatypes::xsd::YearMonthDuration>() == value);

    std::cout<< lit5 <<std::endl;

    //default values
    tm.tm_year = 2083;  // year
    tm.tm_mon = 4;  // month
    tm.tm_mday = 1;  // day
    tm.tm_sec = 0;   // seconds of minutes
    tm.tm_min = 0;   // minutes of hour
    tm.tm_hour = 0; // hours of day
    tm.tm_isdst = -1; // daylight saving

    value = mktime(&tm);

    auto lit6 = Literal::make<datatypes::xsd::YearMonthDuration>(value);
    CHECK(lit6.value<datatypes::xsd::YearMonthDuration>() == value);

    std::cout<< lit6 <<std::endl;

    //default values
    tm.tm_year = 2000;  // year
    tm.tm_mon = 1000;  // month
    tm.tm_mday = 1;  // day
    tm.tm_sec = 0;   // seconds of minutes
    tm.tm_min = 0;   // minutes of hour
    tm.tm_hour = 0; // hours of day
    tm.tm_isdst = -1; // daylight saving

    value = mktime(&tm);

    auto lit7 = Literal::make<datatypes::xsd::YearMonthDuration>(value);
    CHECK(lit7.value<datatypes::xsd::YearMonthDuration>() == value);

    std::cout<< lit7 <<std::endl;

    //default values
    tm.tm_year = 2;  // year
    tm.tm_mon = 13;  // month
    tm.tm_mday = 1;  // day
    tm.tm_sec = 0;   // seconds of minutes
    tm.tm_min = 0;   // minutes of hour
    tm.tm_hour = 0; // hours of day
    tm.tm_isdst = -1; // daylight saving

    value = mktime(&tm);

    auto lit8 = Literal::make<datatypes::xsd::YearMonthDuration>(value);
    CHECK(lit8.value<datatypes::xsd::YearMonthDuration>() == value);

    std::cout<< lit8 <<std::endl;

    //default values
    tm.tm_year = 3;  // year
    tm.tm_mon = 1;  // month
    tm.tm_mday = 1;  // day
    tm.tm_sec = 0;   // seconds of minutes
    tm.tm_min = 0;   // minutes of hour
    tm.tm_hour = 0; // hours of day
    tm.tm_isdst = -1; // daylight saving

    value = mktime(&tm);

    auto lit9 = Literal::make<datatypes::xsd::YearMonthDuration>(value);
    CHECK(lit9.value<datatypes::xsd::YearMonthDuration>() == value);

    std::cout<< lit9 <<std::endl;

    //default values
    tm.tm_year = 2;  // year
    tm.tm_mon = 11;  // month
    tm.tm_mday = 1;  // day
    tm.tm_sec = 0;   // seconds of minutes
    tm.tm_min = 0;   // minutes of hour
    tm.tm_hour = 0; // hours of day
    tm.tm_isdst = -1; // daylight saving

    value = mktime(&tm);

    auto lit10 = Literal::make<datatypes::xsd::YearMonthDuration>(value);
    CHECK(lit10.value<datatypes::xsd::YearMonthDuration>() == value);

    std::cout<< lit10 <<std::endl;

    //default values
    tm.tm_year = 3;  // year
    tm.tm_mon = 0;  // month
    tm.tm_mday = 1;  // day
    tm.tm_sec = 0;   // seconds of minutes
    tm.tm_min = 0;   // minutes of hour
    tm.tm_hour = 0; // hours of day
    tm.tm_isdst = -1; // daylight saving

    value = mktime(&tm);

    auto lit11 = Literal::make<datatypes::xsd::YearMonthDuration>(value);
    CHECK(lit11.value<datatypes::xsd::YearMonthDuration>() == value);

    std::cout<< lit11 <<std::endl;

    //default values
    tm.tm_year = 2;  // year
    tm.tm_mon = 12;  // month
    tm.tm_mday = 1;  // day
    tm.tm_sec = 0;   // seconds of minutes
    tm.tm_min = 0;   // minutes of hour
    tm.tm_hour = 0; // hours of day
    tm.tm_isdst = -1; // daylight saving

    value = mktime(&tm);

    auto lit12 = Literal::make<datatypes::xsd::YearMonthDuration>(value);
    CHECK(lit12.value<datatypes::xsd::YearMonthDuration>() == value);

    std::cout<< lit12 <<std::endl;

    CHECK(lit1 != lit2);
    CHECK(lit1 != lit3);
    CHECK(lit6 == lit7);
    CHECK(lit8 == lit9);
    CHECK(lit11 == lit12);

    // suppress warnings regarding attribute ‘nodiscard’
    Literal no_discard_dummy;

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("22-04-2022", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("22-04-2022T10:10:10", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("22:04:30", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("22-04-2022T10:10:10Z", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("abc", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("p1d", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("pT1S", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("PT1s", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("P1Y1H", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("P1Y1MT1S", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("P2M\n", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("P1Y\t", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("P1Y1M\r", type_iri), "XSD Parsing Error", std::runtime_error);
}
