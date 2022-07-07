#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("Datatype Duration") {

    constexpr auto correct_type_iri_cstr = "http://www.w3.org/2001/XMLSchema#duration";

    CHECK(std::string(datatypes::xsd::Duration::identifier) == correct_type_iri_cstr);

    auto type_iri = IRI(datatypes::xsd::Duration::identifier);
    CHECK(type_iri.identifier() == correct_type_iri_cstr);

    using type = datatypes::xsd::Duration::cpp_type;

    CHECK(std::is_same_v<type, time_t>);

    struct tm tm{};

    //default values
    tm.tm_year = 1;  // year
    tm.tm_mon = 1;  // month
    tm.tm_mday = 1;  // day
    tm.tm_sec = 1;   // seconds of minutes
    tm.tm_min = 1;   // minutes of hour
    tm.tm_hour = 1; // hours of day
    tm.tm_isdst = -1; // daylight saving

    auto value = mktime(&tm);

    auto lit1 = Literal::make<datatypes::xsd::Duration>(value);
    CHECK(lit1.value<datatypes::xsd::Duration>() == value);

    std::cout<< lit1 <<std::endl;

    //default values
    tm.tm_year = 1;  // year
    tm.tm_mon = 0;  // month
    tm.tm_mday = 0;  // day
    tm.tm_sec = 0;   // seconds of minutes
    tm.tm_min = 0;   // minutes of hour
    tm.tm_hour = 0; // hours of day
    tm.tm_isdst = -1; // daylight saving

    value = mktime(&tm);

    auto lit2 = Literal::make<datatypes::xsd::Duration>(value);
    CHECK(lit2.value<datatypes::xsd::Duration>() == value);

    std::cout<< lit2 <<std::endl;

    //default values
    tm.tm_year = 0;  // year
    tm.tm_mon = 1;  // month
    tm.tm_mday = 0;  // day
    tm.tm_sec = 0;   // seconds of minutes
    tm.tm_min = 0;   // minutes of hour
    tm.tm_hour = 0; // hours of day
    tm.tm_isdst = -1; // daylight saving

    value = mktime(&tm);

    auto lit3 = Literal::make<datatypes::xsd::Duration>(value);
    CHECK(lit3.value<datatypes::xsd::Duration>() == value);

    std::cout<< lit3 <<std::endl;

    //default values
    tm.tm_year = 0;  // year
    tm.tm_mon = 0;  // month
    tm.tm_mday = 1;  // day
    tm.tm_sec = 0;   // seconds of minutes
    tm.tm_min = 0;   // minutes of hour
    tm.tm_hour = 0; // hours of day
    tm.tm_isdst = -1; // daylight saving

    value = mktime(&tm);

    auto lit4 = Literal::make<datatypes::xsd::Duration>(value);
    CHECK(lit4.value<datatypes::xsd::Duration>() == value);

    std::cout<< lit4 <<std::endl;

    //default values
    tm.tm_year = 0;  // year
    tm.tm_mon = 0;  // month
    tm.tm_mday = 0;  // day
    tm.tm_sec = 1;   // seconds of minutes
    tm.tm_min = 0;   // minutes of hour
    tm.tm_hour = 0; // hours of day
    tm.tm_isdst = 0; // daylight saving

    value = mktime(&tm);

    auto lit5 = Literal::make<datatypes::xsd::Duration>(value);
    CHECK(lit5.value<datatypes::xsd::Duration>() == value);

    std::cout<< lit5 <<std::endl;

    //default values
    tm.tm_year = 0;  // year
    tm.tm_mon = 0;  // month
    tm.tm_mday = 0;  // day
    tm.tm_sec = 0;   // seconds of minutes
    tm.tm_min = 1;   // minutes of hour
    tm.tm_hour = 0; // hours of day
    tm.tm_isdst = -1; // daylight saving

    value = mktime(&tm);

    auto lit6 = Literal::make<datatypes::xsd::Duration>(value);
    CHECK(lit6.value<datatypes::xsd::Duration>() == value);

    std::cout<< lit6 <<std::endl;

    //default values
    tm.tm_year = 0;  // year
    tm.tm_mon = 0;  // month
    tm.tm_mday = 0;  // day
    tm.tm_sec = 0;   // seconds of minutes
    tm.tm_min = 0;   // minutes of hour
    tm.tm_hour = 1; // hours of day
    tm.tm_isdst = -1; // daylight saving

    value = mktime(&tm);

    auto lit7 = Literal::make<datatypes::xsd::Duration>(value);
    CHECK(lit7.value<datatypes::xsd::Duration>() == value);

    std::cout<< lit7 <<std::endl;

    //default values
    tm.tm_year = 1;  // year
    tm.tm_mon = 1;  // month
    tm.tm_mday = 0;  // day
    tm.tm_sec = 0;   // seconds of minutes
    tm.tm_min = 0;   // minutes of hour
    tm.tm_hour = 0; // hours of day
    tm.tm_isdst = -1; // daylight saving

    value = mktime(&tm);

    auto lit8 = Literal::make<datatypes::xsd::Duration>(value);
    CHECK(lit8.value<datatypes::xsd::Duration>() == value);

    std::cout<< lit8 <<std::endl;

    //default values
    tm.tm_year = 1;  // year
    tm.tm_mon = 0;  // month
    tm.tm_mday = 1;  // day
    tm.tm_sec = 0;   // seconds of minutes
    tm.tm_min = 0;   // minutes of hour
    tm.tm_hour = 0; // hours of day
    tm.tm_isdst = -1; // daylight saving

    value = mktime(&tm);

    auto lit9 = Literal::make<datatypes::xsd::Duration>(value);
    CHECK(lit9.value<datatypes::xsd::Duration>() == value);

    std::cout<< lit9 <<std::endl;

    //default values
    tm.tm_year = 1;  // year
    tm.tm_mon = 0;  // month
    tm.tm_mday = 0;  // day
    tm.tm_sec = 1;   // seconds of minutes
    tm.tm_min = 0;   // minutes of hour
    tm.tm_hour = 0; // hours of day
    tm.tm_isdst = -1; // daylight saving

    value = mktime(&tm);

    auto lit10 = Literal::make<datatypes::xsd::Duration>(value);
    CHECK(lit10.value<datatypes::xsd::Duration>() == value);

    std::cout<< lit10 <<std::endl;

    //default values
    tm.tm_year = 1;  // year
    tm.tm_mon = 0;  // month
    tm.tm_mday = 0;  // day
    tm.tm_sec = 0;   // seconds of minutes
    tm.tm_min = 1;   // minutes of hour
    tm.tm_hour = 0; // hours of day
    tm.tm_isdst = -1; // daylight saving

    value = mktime(&tm);

    auto lit11 = Literal::make<datatypes::xsd::Duration>(value);
    CHECK(lit11.value<datatypes::xsd::Duration>() == value);

    std::cout<< lit11 <<std::endl;

    //default values
    tm.tm_year = 1;  // year
    tm.tm_mon = 0;  // month
    tm.tm_mday = 0;  // day
    tm.tm_sec = 0;   // seconds of minutes
    tm.tm_min = 0;   // minutes of hour
    tm.tm_hour = 1; // hours of day
    tm.tm_isdst = -1; // daylight saving

    value = mktime(&tm);

    auto lit12 = Literal::make<datatypes::xsd::Duration>(value);
    CHECK(lit12.value<datatypes::xsd::Duration>() == value);

    std::cout<< lit12 <<std::endl;

    //default values
    tm.tm_year = 1;  // year
    tm.tm_mon = 0;  // month
    tm.tm_mday = 31;  // day
    tm.tm_sec = 0;   // seconds of minutes
    tm.tm_min = 0;   // minutes of hour
    tm.tm_hour = 0; // hours of day
    tm.tm_isdst = -1; // daylight saving

    value = mktime(&tm);

    auto lit13 = Literal::make<datatypes::xsd::Duration>(value);
    CHECK(lit13.value<datatypes::xsd::Duration>() == value);

    std::cout<< lit13 <<std::endl;

    CHECK(lit1 != lit2);
    CHECK(lit2 != lit3);
    CHECK(lit8 == lit13);

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

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("P2M\n", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("P1Y\t", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("P1Y1M\r", type_iri), "XSD Parsing Error", std::runtime_error);
}
