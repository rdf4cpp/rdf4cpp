#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

#include <bitset>

using namespace rdf4cpp::rdf::datatypes::xsd;
using rdf4cpp::rdf::datatypes::registry::HexBinaryRepr;

TEST_SUITE("xsd:hexBinary") {
    TEST_CASE("canonical") {
        SUBCASE("a") {
            std::string_view const h = "0FB7";
            auto const x = HexBinary::from_string(h);
            CHECK(x == HexBinaryRepr{{std::byte{0xB7}, std::byte{0xF}}});

            auto const back = x.to_encoded();

            CHECK(h == back);
        }

        SUBCASE("b") {
            std::string_view const h = "FB7";
            auto const x = HexBinary::from_string(h);
            CHECK(x == HexBinaryRepr{{std::byte{0xB7}, std::byte{0xF}}});

            auto const back = x.to_encoded();

            CHECK(back == "0FB7");
        }

        SUBCASE("c") {
            std::string_view const h = "0ab89";
            auto const x = HexBinary::from_string(h);
            CHECK(x == HexBinaryRepr{{std::byte{0x89}, std::byte{0xAB}}});

            auto const back = x.to_encoded();

            CHECK(back == "AB89");
        }

        SUBCASE("d") {
            std::string_view const h = "00ab89";
            auto const x = HexBinary::from_string(h);
            CHECK(x == HexBinaryRepr{{std::byte{0x89}, std::byte{0xAB}}});

            auto const back = x.to_encoded();

            CHECK(back == "AB89");
        }

        SUBCASE("e") {
            std::string_view const h = "0000";
            auto const x = HexBinary::from_string(h);
            CHECK(x == HexBinaryRepr{});

            auto const back = x.to_encoded();
            CHECK(back == "0");
        }
    }

    TEST_CASE("invalid chars") {
        std::string_view const h = "0FXB7";
        CHECK_THROWS(HexBinary::from_string(h));
    }
}
