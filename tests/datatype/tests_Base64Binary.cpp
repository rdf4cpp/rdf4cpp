#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include "rdf4cpp.hpp"

using namespace rdf4cpp::rdf::datatypes::xsd;

// ascii -> value
// note: modified to decode '=' to 0 because the original table is not supposed to be used to decode padding hextets
// as they need special handling during decoding, here however when extracting hextets we are expecting padding hextets to have a value of 0
static constexpr std::array<uint8_t, 128> decode_lut{127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,
                                                     127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,
                                                     127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,   62,  127,  127,  127,   63,
                                                      52,   53,   54,   55,   56,   57,   58,   59,   60,   61,  127,  127,  127,    0,  127,  127,
                                                     127,    0,    1,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,   12,   13,   14,
                                                      15,   16,   17,   18,   19,   20,   21,   22,   23,   24,   25,  127,  127,  127,  127,  127,
                                                     127,   26,   27,   28,   29,   30,   31,   32,   33,   34,   35,   36,   37,   38,   39,   40,
                                                      41,   42,   43,   44,   45,   46,   47,   48,   49,   50,   51,  127,  127,  127,  127,  127};

static std::byte base64_decode_single(char const c) {
    auto const decoded = decode_lut[static_cast<size_t>(c)];
    REQUIRE(decoded != 127);
    return static_cast<std::byte>(decoded);
}

TEST_SUITE("base64Binary") {
    TEST_CASE("canonicalization") {
        SUBCASE("zero pad") {
            std::string_view const s = "U29tZSBzdHJpbmcgaGVy";
            auto const b64 = Base64Binary::from_string(s);
            CHECK(b64.n_hextets() == s.size());

            auto const back = b64.to_encoded();
            CHECK(s == back);

            for (size_t ix = 0; ix < b64.n_hextets(); ++ix) {
                CHECK(base64_decode_single(s[ix]) == b64.hextet(ix));
            }
        }

        SUBCASE("one pad") {
            std::string_view const s = "SGVsbG8gV29ybGQ=";
            auto const b64 = Base64Binary::from_string(s);
            CHECK(b64.n_hextets() == s.size());
            CHECK(b64.n_bytes() == std::string_view{"Hello World"}.size());

            auto const back = b64.to_encoded();
            CHECK(s == back);

            for (size_t ix = 0; ix < b64.n_hextets(); ++ix) {
                CHECK(base64_decode_single(s[ix]) == b64.hextet(ix));
            }
        }

        SUBCASE("two pad") {
            std::string_view const s = "U29tZSBzdHJpbmcgaGVyZQ==";
            auto const b64 = Base64Binary::from_string(s);
            CHECK(b64.n_hextets() == s.size());

            auto const back = b64.to_encoded();
            CHECK(s == back);

            for (size_t ix = 0; ix < b64.n_hextets(); ++ix) {
                CHECK(base64_decode_single(s[ix]) == b64.hextet(ix));
            }
        }

        SUBCASE("spaces") {
            std::string_view const s = "U29tZ S B z d HJpbmcgaGVyZQ==";
            auto const b64 = Base64Binary::from_string(s);
            CHECK(b64.n_hextets() == s.size() - 5);

            auto const back = b64.to_encoded();
            CHECK(back == "U29tZSBzdHJpbmcgaGVyZQ==");
        }
    }

    TEST_CASE("invalid intermediate spaces") {
        std::string_view const s = "U29tZ  SBzdHJpb mcga GVyZQ==";
        CHECK_THROWS(Base64Binary::from_string(s));
    }

    TEST_CASE("trailing space") {
        std::string_view const s = "U29tZSBzdHJpbmcgaGVyZQ== ";
        CHECK_THROWS(Base64Binary::from_string(s));
    }

    TEST_CASE("leading space") {
        std::string_view const s = " U29tZSBzdHJpbmcgaGVyZQ==";
        CHECK_THROWS(Base64Binary::from_string(s));
    }

    TEST_CASE("invalid pad") {
        std::string_view const s = " U29tZSBzdHJpbmcgaGVyZQ=a";
        CHECK_THROWS(Base64Binary::from_string(s));
    }

    TEST_CASE("invalid len") {
        std::string_view const s = " U29ta";
        CHECK_THROWS(Base64Binary::from_string(s));
    }

    TEST_CASE("invalid chars") {
        std::string_view const s = " U2#9";
        CHECK_THROWS(Base64Binary::from_string(s));
    }
}
