#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf/util/BigDecimal.hpp>

using Dec = rdf4cpp::rdf::util::BigDecimal<>;
using RoundingMode = rdf4cpp::rdf::util::RoundingMode;

TEST_CASE("basics") {
    SUBCASE("ctor and compare") {
        Dec d{500, 1};
        CHECK(d > Dec{500, 1, true});
        CHECK(Dec{500, 1, true} < d);
        CHECK(Dec{50, 0} == d);
        CHECK(d > Dec{40, 0});
        CHECK(d < Dec{60, 0});
        CHECK(Dec{40, 0} < d);
        CHECK(Dec{60, 0} > d);
        CHECK(d != Dec{60, 1});
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
        CHECK(-Dec{1, 0} == Dec{1, 0, true});
        CHECK(-Dec{1, 5} == Dec{1, 5, true});
        CHECK(-Dec{1, 0, true} == Dec{1, 0});
        CHECK(-Dec{1, 5, true} == Dec{1, 5});
    }
    SUBCASE("+") {
        CHECK((Dec{1, 0} + Dec{2, 0}) == Dec{3, 0});
        CHECK((Dec{100, 1} + Dec{200, 1}) == Dec{300, 1});
        CHECK((Dec{1, 0} + Dec{200, 2}) == Dec{300, 2});
        CHECK((Dec{1, 2} + Dec{2, 0}) == Dec{201, 2});
        CHECK((Dec{1, 0} + Dec{2, 0, true}) == Dec{1, 0, true});
        CHECK((Dec{1, 0} + Dec{1, 0, true}) == Dec{0, 0});
        CHECK((Dec{1, 0, true} + Dec{2, 0, true}) == Dec{3, 0, true});
        CHECK((Dec{2, 0} + Dec{1, 0, true}) == Dec{1, 0});
        CHECK((Dec{1, 0, true} + Dec{2, 0}) == Dec{1, 0});
        CHECK((Dec{2, 0, true} + Dec{1, 0}) == Dec{1, 0, true});
    }
    SUBCASE("-") {
        CHECK((Dec{1, 0} - Dec{2, 0}) == Dec{1, 0, true});  // implemented via + and unary -
    }
    SUBCASE("*") {
        CHECK((Dec{2, 0} * Dec{2, 0}) == Dec{4, 0});
        CHECK((Dec{20, 0} * Dec{20, 0}) == Dec{400, 0});
        CHECK((Dec{2, 1} * Dec{2, 0}) == Dec{4, 1});
        CHECK((Dec{2, 1} * Dec{2, 1}) == Dec{4, 2});
        CHECK_THROWS_AS((Dec{2, std::numeric_limits<uint32_t>::max()} * Dec{2, std::numeric_limits<uint32_t>::max()}), std::overflow_error);
        CHECK((Dec{2, 1, true} * Dec{2, 1}) == Dec{4, 2, true});
        CHECK((Dec{2, 1} * Dec{2, 1, true}) == Dec{4, 2, true});
        CHECK((Dec{2, 1, true} * Dec{2, 1, true}) == Dec{4, 2, false});
    }
    SUBCASE("/") {
        CHECK((Dec{2, 0} / Dec{2, 0}) == Dec{1, 0});
        CHECK((Dec{2, 0, true} / Dec{2, 0}) == Dec{1, 0, true});
        CHECK((Dec{2, 0} / Dec{2, 0, true}) == Dec{1, 0, true});
        CHECK((Dec{2, 0, true} / Dec{2, 0, true}) == Dec{1, 0});
        CHECK_THROWS_AS((Dec{1, 0} / Dec{3, 0}), std::overflow_error);
        CHECK((Dec{1, 0}.divide(Dec{3, 0}, 2, RoundingMode::Floor)) == Dec{33, 2});
        CHECK((Dec{1, 0}.divide(Dec{3, 0}, 2, RoundingMode::Ceil)) == Dec{34, 2});
        CHECK((Dec{1, 0}.divide(Dec{3, 0}, 2, RoundingMode::Round)) == Dec{33, 2});
        CHECK((Dec{2, 0}.divide(Dec{3, 0}, 2, RoundingMode::Round)) == Dec{67, 2});
        CHECK((Dec{1, 0}.divide(Dec{3, 0, true}, 2, RoundingMode::Floor)) == Dec{33, 2, true});
    }
    SUBCASE("precision") {
        CHECK(((Dec{1, 0} + Dec{2, 0} + Dec{3, 0}) / Dec{3, 0}) == Dec{2, 0});
    }
}

TEST_CASE("conversion") {
    SUBCASE("std::string") {
        CHECK(static_cast<std::string>(Dec{51, 1}) == "5.1");
        CHECK(static_cast<std::string>(Dec{9514, 2, true}) == "-95.14");
        CHECK(static_cast<std::string>(Dec{9514, 0, true}) == "-9514.0");
        CHECK(static_cast<std::string>(Dec{9514, 4, true}) == "-0.9514");
        CHECK(static_cast<std::string>(Dec{9514, 6, true}) == "-0.009514");
    }
    SUBCASE("writing") {
        std::stringstream str{};
        str << Dec{50, 1};
        CHECK(str.view() == "5.0");
        // uses string conversion, so no more tests here
    }
    SUBCASE("double") {
        CHECK(static_cast<double>(Dec{50, 0}) == 50.0);
        CHECK(static_cast<double>(Dec{500, 1}) == 50.0);
    }
}
