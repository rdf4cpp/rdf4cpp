#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("Datatype DayTimeDuration") {

    constexpr auto correct_type_iri_cstr = "http://www.w3.org/2001/XMLSchema#dayTimeDuration";

    CHECK(std::string(datatypes::xsd::DayTimeDuration::identifier) == correct_type_iri_cstr);

    auto type_iri = IRI(datatypes::xsd::DayTimeDuration::identifier);
    CHECK(type_iri.identifier() == correct_type_iri_cstr);

    using type = datatypes::xsd::DayTimeDuration::cpp_type;

    CHECK(std::is_same_v<type, time_t>);

    struct tm tm{};

    /* By default these values should be set as following for proper creation of lexical form.
     * tm.tm_year = 0;  // year
     * tm.tm_mon = 0;  // month
     */

    //default values
    tm.tm_year = 0;  // year
    tm.tm_mon = 0;  // month
    tm.tm_mday = 1;  // day
    tm.tm_sec = 0;   // seconds of minutes
    tm.tm_min = 0;   // minutes of hour
    tm.tm_hour = 0; // hours of day
    tm.tm_isdst = -1; // daylight saving

    auto value = mktime(&tm);

    auto lit1 = Literal::make<datatypes::xsd::DayTimeDuration>(value);
    CHECK(lit1.value<datatypes::xsd::DayTimeDuration>() == value);

    std::cout<< lit1 <<std::endl;

    //default values
    tm.tm_year = 0;  // year
    tm.tm_mon = 0;  // month
    tm.tm_mday = 0;  // day
    tm.tm_sec = 1;   // seconds of minutes
    tm.tm_min = 0;   // minutes of hour
    tm.tm_hour = 0; // hours of day
    tm.tm_isdst = -1; // daylight saving

    value = mktime(&tm);

    auto lit2 = Literal::make<datatypes::xsd::DayTimeDuration>(value);
    CHECK(lit2.value<datatypes::xsd::DayTimeDuration>() == value);

    std::cout<< lit2 <<std::endl;

    //default values
    tm.tm_year = 0;  // year
    tm.tm_mon = 0;  // month
    tm.tm_mday = 0;  // day
    tm.tm_sec = 0;   // seconds of minutes
    tm.tm_min = 1;   // minutes of hour
    tm.tm_hour = 0; // hours of day
    tm.tm_isdst = -1; // daylight saving

    value = mktime(&tm);

    auto lit3 = Literal::make<datatypes::xsd::DayTimeDuration>(value);
    CHECK(lit3.value<datatypes::xsd::DayTimeDuration>() == value);

    std::cout<< lit3 <<std::endl;

    //default values
    tm.tm_year = 0;  // year
    tm.tm_mon = 0;  // month
    tm.tm_mday = 0;  // day
    tm.tm_sec = 0;   // seconds of minutes
    tm.tm_min = 0;   // minutes of hour
    tm.tm_hour = 1; // hours of day
    tm.tm_isdst = -1; // daylight saving

    value = mktime(&tm);

    auto lit4 = Literal::make<datatypes::xsd::DayTimeDuration>(value);
    CHECK(lit4.value<datatypes::xsd::DayTimeDuration>() == value);

    std::cout<< lit4 <<std::endl;

    //default values
    tm.tm_year = 0;  // year
    tm.tm_mon = 0;  // month
    tm.tm_mday = 1;  // day
    tm.tm_sec = 1;   // seconds of minutes
    tm.tm_min = 1;   // minutes of hour
    tm.tm_hour = 1; // hours of day
    tm.tm_isdst = 0; // daylight saving

    value = mktime(&tm);

    auto lit5 = Literal::make<datatypes::xsd::DayTimeDuration>(value);
    CHECK(lit5.value<datatypes::xsd::DayTimeDuration>() == value);

    std::cout<< lit5 <<std::endl;

    //default values
    tm.tm_year = 0;  // year
    tm.tm_mon = 0;  // month
    tm.tm_mday = 1;  // day
    tm.tm_sec = 60;   // seconds of minutes
    tm.tm_min = 0;   // minutes of hour
    tm.tm_hour = 0; // hours of day
    tm.tm_isdst = -1; // daylight saving

    value = mktime(&tm);

    auto lit6 = Literal::make<datatypes::xsd::DayTimeDuration>(value);
    CHECK(lit6.value<datatypes::xsd::DayTimeDuration>() == value);

    std::cout<< lit6 <<std::endl;

    //default values
    tm.tm_year = 0;  // year
    tm.tm_mon = 0;  // month
    tm.tm_mday = 1;  // day
    tm.tm_sec = 0;   // seconds of minutes
    tm.tm_min = 1;   // minutes of hour
    tm.tm_hour = 0; // hours of day
    tm.tm_isdst = -1; // daylight saving

    value = mktime(&tm);

    auto lit7 = Literal::make<datatypes::xsd::DayTimeDuration>(value);
    CHECK(lit7.value<datatypes::xsd::DayTimeDuration>() == value);

    std::cout<< lit7 <<std::endl;

    //default values
    tm.tm_year = 0;  // year
    tm.tm_mon = 0;  // month
    tm.tm_mday = 1;  // day
    tm.tm_sec = 0;   // seconds of minutes
    tm.tm_min = 60;   // minutes of hour
    tm.tm_hour = 0; // hours of day
    tm.tm_isdst = -1; // daylight saving

    value = mktime(&tm);

    auto lit8 = Literal::make<datatypes::xsd::DayTimeDuration>(value);
    CHECK(lit8.value<datatypes::xsd::DayTimeDuration>() == value);

    std::cout<< lit8 <<std::endl;

    //default values
    tm.tm_year = 0;  // year
    tm.tm_mon = 0;  // month
    tm.tm_mday = 1;  // day
    tm.tm_sec = 0;   // seconds of minutes
    tm.tm_min = 0;   // minutes of hour
    tm.tm_hour = 1; // hours of day
    tm.tm_isdst = -1; // daylight saving

    value = mktime(&tm);

    auto lit9 = Literal::make<datatypes::xsd::DayTimeDuration>(value);
    CHECK(lit9.value<datatypes::xsd::DayTimeDuration>() == value);

    std::cout<< lit9 <<std::endl;

    //default values
    tm.tm_year = 0;  // year
    tm.tm_mon = 0;  // month
    tm.tm_mday = 30;  // day
    tm.tm_sec = 3600;   // seconds of minutes
    tm.tm_min = 60;   // minutes of hour
    tm.tm_hour = 23; // hours of day
    tm.tm_isdst = -1; // daylight saving

    value = mktime(&tm);

    auto lit10 = Literal::make<datatypes::xsd::DayTimeDuration>(value);
    CHECK(lit10.value<datatypes::xsd::DayTimeDuration>() == value);

    std::cout<< lit10 <<std::endl;

    //default values
    tm.tm_year = 0;  // year
    tm.tm_mon = 0;  // month
    tm.tm_mday = 60;  // day
    tm.tm_sec = 0;   // seconds of minutes
    tm.tm_min = 0;   // minutes of hour
    tm.tm_hour = 0; // hours of day
    tm.tm_isdst = -1; // daylight saving

    value = mktime(&tm);

    auto lit11 = Literal::make<datatypes::xsd::DayTimeDuration>(value);
    CHECK(lit11.value<datatypes::xsd::DayTimeDuration>() == value);

    std::cout<< lit11 <<std::endl;

    //default values
    tm.tm_year = 0;  // year
    tm.tm_mon = 0;  // month
    tm.tm_mday = 600;  // day
    tm.tm_sec = 0;   // seconds of minutes
    tm.tm_min = 0;   // minutes of hour
    tm.tm_hour = 0; // hours of day
    tm.tm_isdst = -1; // daylight saving

    value = mktime(&tm);

    auto lit12 = Literal::make<datatypes::xsd::DayTimeDuration>(value);
    CHECK(lit12.value<datatypes::xsd::DayTimeDuration>() == value);

    std::cout<< lit12 <<std::endl;

    CHECK(lit1 != lit2);
    CHECK(lit1 != lit3);
    CHECK(lit6 == lit7);
    CHECK(lit8 == lit9);

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

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("P1DT1H\n", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("PT1S\t", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("PT1M\r", type_iri), "XSD Parsing Error", std::runtime_error);
}