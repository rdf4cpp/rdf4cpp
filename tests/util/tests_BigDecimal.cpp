#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf/util/BigDecimal.hpp>

using Dec = rdf4cpp::rdf::util::BigDecimal<>;

TEST_CASE("basics") {
    Dec d{500, 1};
    CHECK(static_cast<double>(d) == 50.0);
    CHECK(d > Dec{500, 1, true});
    CHECK(Dec{500, 1, true} < d);
    CHECK(Dec{50,0} == d);
    CHECK(d > Dec{40,0});
    CHECK(d < Dec{60,0});
    CHECK(Dec{40,0} < d);
    CHECK(Dec{60,0} > d);
    CHECK(d != Dec{60,1});
    d.normalize();
    CHECK(static_cast<double>(d) == 50.0);
    CHECK(d.get_exponent() == 0);
    d = Dec{51,1};
    d.normalize();
    CHECK(d == Dec{51,1});
    CHECK(d.get_exponent() == 1);
    CHECK(static_cast<std::string>(d) == "5.1");
    CHECK(static_cast<std::string>(Dec{9514,2,true}) == "-95.14");
}
