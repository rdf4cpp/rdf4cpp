#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf/util/BigDecimal.hpp>

using Dec = rdf4cpp::rdf::util::BigDecimal<>;
using RoundingMode = rdf4cpp::rdf::util::RoundingMode;
using Sign = rdf4cpp::rdf::util::Sign;

TEST_CASE("basics") {
    SUBCASE("ctor and compare") {
        static_assert(rdf4cpp::rdf::util::BigDecimalBaseType<uint32_t>);
        static_assert(!rdf4cpp::rdf::util::BigDecimalBaseType<uint8_t>);
        static_assert(!rdf4cpp::rdf::util::BigDecimalBaseType<int32_t>);
        static_assert(rdf4cpp::rdf::util::BigDecimalBaseType<uint64_t>);
        static_assert(rdf4cpp::rdf::util::BigDecimalBaseType<boost::multiprecision::cpp_int>);
        Dec d{500, 1};
        CHECK(d > Dec{500, 1, Sign::Negative});
        CHECK(Dec{500, 1, Sign::Negative} < d);
        CHECK(Dec{50, 0} == d);
        CHECK(d > Dec{40, 0});
        CHECK(d < Dec{60, 0});
        CHECK(Dec{40, 0} < d);
        CHECK(Dec{60, 0} > d);
        CHECK(d != Dec{60, 1});
        CHECK(d == Dec{-500, 1, Sign::Negative});
        CHECK(Dec{0, 0, Sign::Negative} == Dec{0, 0, Sign::Positive});
    }
    SUBCASE("normalize") {
        Dec d{500, 1};
        d.normalize();
        CHECK(d == Dec{50, 0});
        CHECK(d.get_exponent() == 0);
        d = Dec{51, 1};
        d.normalize();
        CHECK(d == Dec{51, 1});
        CHECK(d.get_exponent() == 1);
    }
}

TEST_CASE("arithmetic") {
    SUBCASE("unary -") {
        CHECK(-Dec{1, 0} == Dec{1, 0, Sign::Negative});
        CHECK(-Dec{1, 5} == Dec{1, 5, Sign::Negative});
        CHECK(-Dec{1, 0, Sign::Negative} == Dec{1, 0});
        CHECK(-Dec{1, 5, Sign::Negative} == Dec{1, 5});
    }
    SUBCASE("+") {
        CHECK((Dec{1, 0} + Dec{2, 0}) == Dec{3, 0});
        CHECK((Dec{100, 1} + Dec{200, 1}) == Dec{300, 1});
        CHECK((Dec{1, 0} + Dec{200, 2}) == Dec{300, 2});
        CHECK((Dec{1, 2} + Dec{2, 0}) == Dec{201, 2});
        CHECK((Dec{1, 0} + Dec{2, 0, Sign::Negative}) == Dec{1, 0, Sign::Negative});
        CHECK((Dec{1, 0} + Dec{1, 0, Sign::Negative}) == Dec{0, 0});
        CHECK((Dec{1, 0, Sign::Negative} + Dec{2, 0, Sign::Negative}) == Dec{3, 0, Sign::Negative});
        CHECK((Dec{2, 0} + Dec{1, 0, Sign::Negative}) == Dec{1, 0});
        CHECK((Dec{1, 0, Sign::Negative} + Dec{2, 0}) == Dec{1, 0});
        CHECK((Dec{2, 0, Sign::Negative} + Dec{1, 0}) == Dec{1, 0, Sign::Negative});
    }
    SUBCASE("-") {
        CHECK((Dec{1, 0} - Dec{2, 0}) == Dec{1, 0, Sign::Negative});  // implemented via + and unary -
    }
    SUBCASE("*") {
        CHECK((Dec{2, 0} * Dec{2, 0}) == Dec{4, 0});
        CHECK((Dec{20, 0} * Dec{20, 0}) == Dec{400, 0});
        CHECK((Dec{2, 1} * Dec{2, 0}) == Dec{4, 1});
        CHECK((Dec{2, 1} * Dec{2, 1}) == Dec{4, 2});
        CHECK_THROWS_AS((Dec{2, std::numeric_limits<uint32_t>::max()} * Dec{2, std::numeric_limits<uint32_t>::max()}), std::overflow_error);
        CHECK((Dec{2, 1, Sign::Negative} * Dec{2, 1}) == Dec{4, 2, Sign::Negative});
        CHECK((Dec{2, 1} * Dec{2, 1, Sign::Negative}) == Dec{4, 2, Sign::Negative});
        CHECK((Dec{2, 1, Sign::Negative} * Dec{2, 1, Sign::Negative}) == Dec{4, 2, Sign::Positive});
    }
    SUBCASE("/") {
        CHECK((Dec{2, 0} / Dec{2, 0}) == Dec{1, 0});
        CHECK((Dec{2, 0, Sign::Negative} / Dec{2, 0}) == Dec{1, 0, Sign::Negative});
        CHECK((Dec{2, 0} / Dec{2, 0, Sign::Negative}) == Dec{1, 0, Sign::Negative});
        CHECK((Dec{2, 0, Sign::Negative} / Dec{2, 0, Sign::Negative}) == Dec{1, 0});
        CHECK_THROWS_AS((Dec{1, 0} / Dec{3, 0}), std::overflow_error);
        CHECK((Dec{1, 0}.divide(Dec{3, 0}, 2, RoundingMode::Floor)) == Dec{33, 2});
        CHECK((Dec{1, 0}.divide(Dec{3, 0}, 2, RoundingMode::Ceil)) == Dec{34, 2});
        CHECK((Dec{1, 0}.divide(Dec{3, 0}, 2, RoundingMode::Round)) == Dec{33, 2});
        CHECK((Dec{2, 0}.divide(Dec{3, 0}, 2, RoundingMode::Round)) == Dec{67, 2});
        CHECK((Dec{1, 0}.divide(Dec{3, 0, Sign::Negative}, 2, RoundingMode::Floor)) == Dec{33, 2, Sign::Negative});
    }
    SUBCASE("precision") {
        CHECK(((Dec{1, 0} + Dec{2, 0} + Dec{3, 0}) / Dec{3, 0}) == Dec{2, 0});
    }
    SUBCASE("round") {
        CHECK(Dec{51, 1}.round(RoundingMode::Floor) == Dec{5, 0});
        CHECK(Dec{56, 1}.round(RoundingMode::Floor) == Dec{5, 0});
        CHECK(Dec{41, 1}.round(RoundingMode::Ceil) == Dec{5, 0});
        CHECK(Dec{47, 1}.round(RoundingMode::Ceil) == Dec{5, 0});
        CHECK(Dec{51, 1}.round(RoundingMode::Round) == Dec{5, 0});
        CHECK(Dec{48, 1}.round(RoundingMode::Round) == Dec{5, 0});
        CHECK(Dec{549, 2}.round(RoundingMode::Round) == Dec{5, 0});
        CHECK(Dec{450, 2}.round(RoundingMode::Round) == Dec{5, 0});
        CHECK(Dec{5, 0}.round(RoundingMode::Round) == Dec{5, 0});
    }
    SUBCASE("abs") {
        CHECK(Dec{51, 1}.abs() == Dec{51, 1});
        CHECK(Dec{51, 1, Sign::Negative}.abs() == Dec{51, 1});
    }
    SUBCASE("pow") {
        CHECK(Dec{5, 0}.pow(5) == Dec{3125});
        CHECK(Dec{5, 1}.pow(5) == Dec{"0.03125"});
        CHECK(pow(Dec{5, 1}, 0) == Dec{1});
    }
}

TEST_CASE("conversion") {
    SUBCASE("to std::string") {
        CHECK(static_cast<std::string>(Dec{51, 1}) == "5.1");
        CHECK(static_cast<std::string>(Dec{9514, 2, Sign::Negative}) == "-95.14");
        CHECK(static_cast<std::string>(Dec{9514, 0, Sign::Negative}) == "-9514.0");
        CHECK(static_cast<std::string>(Dec{9514, 4, Sign::Negative}) == "-0.9514");
        CHECK(static_cast<std::string>(Dec{9514, 6, Sign::Negative}) == "-0.009514");
        CHECK(static_cast<std::string>(Dec{500, 2}) == "5.0");
        CHECK(static_cast<std::string>(Dec{0, 5}) == "0.0");
        CHECK(static_cast<std::string>(Dec{100, 5}) == "0.001");
        CHECK(static_cast<std::string>(Dec{100, 0}) == "100.0");
    }
    SUBCASE("writing") {
        std::stringstream str{};
        str << Dec{50, 1};
        CHECK(str.view() == "5.0");
        // uses string conversion, so no more tests here
    }
    SUBCASE("from double") {
        CHECK(Dec{50.0} == Dec{50, 0});
        CHECK(Dec{-50.5} == Dec{505, 1, Sign::Negative});
        CHECK(Dec{500000.0} == Dec{500000, 0});
        CHECK(Dec{0.0009765625} == Dec{"0.0009765625"});
        CHECK(Dec{1.0} == Dec{1, 0});
    }
    SUBCASE("to double") {
        CHECK(static_cast<double>(Dec{50, 0}) == 50.0);
        CHECK(static_cast<double>(Dec{500, 1}) == 50.0);
    }
    SUBCASE("to float") {
        CHECK(static_cast<float>(Dec{50, 0}) == 50.0f);
        CHECK(static_cast<float>(Dec{500, 1}) == 50.0f);
    }
    SUBCASE("from string") {
        CHECK(Dec{"5"} == Dec{5, 0});
        CHECK(Dec{"5."} == Dec{5, 0});
        CHECK(Dec{"5.1"} == Dec{51, 1});
        CHECK(Dec{"54.32"} == Dec{5432, 2});
        CHECK(Dec{".54"} == Dec{54, 2});
        CHECK(Dec{"0005.000"} == Dec{5000, 3});
        CHECK(Dec{"0005.000"}.get_exponent() == 3);
        CHECK(Dec{"-5"} == Dec{5, 0, Sign::Negative});
        CHECK(Dec{"-5."} == Dec{5, 0, Sign::Negative});
        CHECK(Dec{"-5.1"} == Dec{51, 1, Sign::Negative});
        CHECK(Dec{"-54.32"} == Dec{5432, 2, Sign::Negative});
        CHECK(Dec{"-.54"} == Dec{54, 2, Sign::Negative});
        CHECK(Dec{"-0005.000"} == Dec{5000, 3, Sign::Negative});
        CHECK(Dec{"-0005.000"}.get_exponent() == 3);
        CHECK(Dec{"0.0"} == Dec{"-0.0"});
        CHECK_THROWS_AS(Dec{"5.5.5"}, std::invalid_argument);
        CHECK_THROWS_AS(Dec{"5.5-5"}, std::invalid_argument);
        CHECK_THROWS_AS(Dec{"5.5+5"}, std::invalid_argument);
        // no e notation allowed by rdf (xml) standard
    }
    SUBCASE("from cpp_int") {
        CHECK(Dec{boost::multiprecision::cpp_int{5}} == Dec{5, 0});
    }
    SUBCASE("to cpp_int") {
        CHECK(static_cast<boost::multiprecision::cpp_int>(Dec{5, 0}) == boost::multiprecision::cpp_int{5});
        CHECK(static_cast<boost::multiprecision::cpp_int>(Dec{59, 1}) == boost::multiprecision::cpp_int{5});
    }
}
