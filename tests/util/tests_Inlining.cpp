#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

#include <limits>

using namespace rdf4cpp::rdf;

TEST_SUITE("inlining util") {
    using namespace datatypes::registry::util;

    TEST_CASE("unsigned smaller into bigger") {
        auto const smaller = std::numeric_limits<uint32_t>::max();
        auto const packed = try_pack_integral<uint64_t, 42>(smaller);
        CHECK(packed.has_value());

        CHECK(*packed < (1ul << 32));

        auto const unpacked = unpack_integral<uint32_t, 42>(*packed);
        CHECK(unpacked == smaller);
    }

    TEST_CASE("signed smaller into bigger") {
        auto const smaller = std::numeric_limits<int32_t>::min();
        auto const packed = try_pack_integral<uint64_t, 42>(smaller);
        CHECK(packed.has_value());

        CHECK(*packed < (1ul << 32));

        auto const unpacked = unpack_integral<int32_t, 42>(*packed);
        CHECK(unpacked == smaller);
    }

    TEST_CASE("pack signed") {
        for (int64_t i = 0; i < (1l << 19); ++i) {
            auto packed = try_pack_integral<uint64_t, 20>(i);
            CHECK(packed.has_value());

            auto unpacked = unpack_integral<int64_t, 20>(*packed);

            REQUIRE(i == unpacked);
        }

        for (int64_t i = -1; i >= -(1l << 19); --i) {
            auto packed = try_pack_integral<uint64_t, 20>(i);
            CHECK(packed.has_value());

            auto unpacked = unpack_signed<int64_t, 20>(*packed);

            REQUIRE(i == unpacked);
        }
    }
}
