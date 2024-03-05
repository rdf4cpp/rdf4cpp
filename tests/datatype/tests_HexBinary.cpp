#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp.hpp>

using namespace rdf4cpp::datatypes::xsd;
using rdf4cpp::datatypes::registry::HexBinaryRepr;

TEST_SUITE("xsd:hexBinary") {
    TEST_CASE("canonicalization") {
        SUBCASE("a") {
            std::string_view const h = "0FB7";
            auto const x = HexBinary::from_string(h);
            CHECK(x == HexBinaryRepr{{std::byte{0xF}, std::byte{0xB7}}});

            auto const back = x.to_encoded();
            CHECK(h == back);

            CHECK(x.n_half_octets() == 4);
            CHECK(x.half_octet(0) == std::byte{0x0});
            CHECK(x.half_octet(1) == std::byte{0xF});
            CHECK(x.half_octet(2) == std::byte{0xB});
            CHECK(x.half_octet(3) == std::byte{0x7});
        }

        SUBCASE("b") {
            std::string_view const h = "FB7";
            auto const x = HexBinary::from_string(h);
            CHECK(x == HexBinaryRepr{{std::byte{0xF}, std::byte{0xB7}}});

            auto const back = x.to_encoded();
            CHECK(back == "0FB7");

            CHECK(x.n_half_octets() == 4);
            CHECK(x.half_octet(0) == std::byte{0x0});
            CHECK(x.half_octet(1) == std::byte{0xF});
            CHECK(x.half_octet(2) == std::byte{0xB});
            CHECK(x.half_octet(3) == std::byte{0x7});
        }

        SUBCASE("c") {
            std::string_view const h = "0ab89";
            auto const x = HexBinary::from_string(h);
            CHECK(x == HexBinaryRepr{{std::byte{0xAB}, std::byte{0x89}}});

            auto const back = x.to_encoded();
            CHECK(back == "AB89");

            CHECK(x.n_half_octets() == 4);
            CHECK(x.half_octet(0) == std::byte{0xA});
            CHECK(x.half_octet(1) == std::byte{0xB});
            CHECK(x.half_octet(2) == std::byte{0x8});
            CHECK(x.half_octet(3) == std::byte{0x9});
        }

        SUBCASE("d") {
            std::string_view const h = "00ab89";
            auto const x = HexBinary::from_string(h);
            CHECK(x == HexBinaryRepr{{std::byte{0xAB}, std::byte{0x89}}});

            auto const back = x.to_encoded();
            CHECK(back == "AB89");

            CHECK(x.n_half_octets() == 4);
            CHECK(x.half_octet(0) == std::byte{0xA});
            CHECK(x.half_octet(1) == std::byte{0xB});
            CHECK(x.half_octet(2) == std::byte{0x8});
            CHECK(x.half_octet(3) == std::byte{0x9});
        }

        SUBCASE("e") {
            std::string_view const h = "0000";
            auto const x = HexBinary::from_string(h);
            CHECK(x == HexBinaryRepr{});

            auto const back = x.to_encoded();
            CHECK(back == "0");

            CHECK(x.n_half_octets() == 0);
        }
    }

    TEST_CASE("invalid chars") {
        std::string_view const h = "0FXB7";
        CHECK_THROWS(HexBinary::from_string(h));
    }
}
