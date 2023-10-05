#include <rdf4cpp/rdf/datatypes/xsd/Base64Binary.hpp>

#include <array>
#include <algorithm>
#include <cstddef>

namespace rdf4cpp::rdf::datatypes::registry {

#ifndef DOXYGEN_PARSER
namespace encode_decode_detail {

// integer value -> ascii base64 character
static constexpr std::array<char, 64> encode_lut{'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
                                                 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
                                                 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
                                                 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
                                                 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2',
                                                 '3', '4', '5', '6', '7', '8', '9', '+', '/'};

// ascii base64 character -> integer value (e.g. 'A' => 0, 'a' => 26)
// see https://en.wikipedia.org/wiki/Base64
// 127 is the error value, meaning the character is invalid for base64 encoding
// the padding char '=' is also mapped to 127 because it is not supposed to be decoded by this table as it needs special handling
static constexpr std::array<uint8_t, 128> decode_lut{127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,
                                                     127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,
                                                     127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,   62,  127,  127,  127,   63,
                                                      52,   53,   54,   55,   56,   57,   58,   59,   60,   61,  127,  127,  127,  127,  127,  127,
                                                     127,    0,    1,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,   12,   13,   14,
                                                      15,   16,   17,   18,   19,   20,   21,   22,   23,   24,   25,  127,  127,  127,  127,  127,
                                                     127,   26,   27,   28,   29,   30,   31,   32,   33,   34,   35,   36,   37,   38,   39,   40,
                                                      41,   42,   43,   44,   45,   46,   47,   48,   49,   50,   51,  127,  127,  127,  127,  127};

static std::array<char, 4> base64_encode(std::byte const h1, std::byte const h2, std::byte const h3) noexcept {
    /*
     *  visualization of the 'combined' variable
     *  put 3 bytes / 8-bit units into it then extract 4 hextets / 6-bit units
     *
     *  | decoded 1                     | decoded 2                     | decoded 3                     | <- first 24 bits / 4 hextets / 3 bytes of combined
     *  | 7   6   5   4   3   2   1   0 | 7   6   5   4   3   2   1   0 | 7   6   5   4   3   2   1   0 |
     *  |           |           |           |           |           |           |           |           |
     *  |           |           |           |           |           |           |           |           |
     *  |           v           |           v           |           v           |           v           |
     *  | 5   4   3   2   1   0 | 5   4   3   2   1   0 | 5   4   3   2   1   0 | 5   4   3   2   1   0 | output hextets, still need to be encoded to ascii (via encode_lut)
     *  | hextet  1             | hextet 2              | hextet 3              | hextet 4              | (only 6 bits used because base64 is a 6 bit encoding)
     *
     *
     *  see https://de.wikipedia.org/wiki/Base64#/media/Datei:Base64-de.png
     */
    uint32_t const combined = static_cast<uint32_t>(h1) << 16 | static_cast<uint32_t>(h2) << 8 | static_cast<uint32_t>(h3);

    // extract hextets out of combined and encode to ascii
    return {encode_lut[(combined >> 18) & 0b0011'1111],
            encode_lut[(combined >> 12) & 0b0011'1111],
            encode_lut[(combined >> 6) & 0b0011'1111],
            encode_lut[combined & 0b0011'1111]};
}

static uint8_t base64_decode_single(char const c) {
    auto const decoded = decode_lut[static_cast<size_t>(c)];
    if (decoded == 127) {
        throw std::runtime_error{"xsd:base64Binary parsing error: invalid digit"};
    }

    return decoded;
}

static std::array<std::byte, 3> base64_decode(char const c1, char const c2, char const c3, char const c4) {
    /*
     *  visualization of the 'combined' variable
     *  put 4 hextets / 6-bit units into it then extract 3 bytes / 8-bit units
     *
     *  | encoded 1             | encoded 2             | encoded 3             | encoded 4             | input chars, decoded with base64_decode_single
     *  | 5   4   3   2   1   0 | 5   4   3   2   1   0 | 5   4   3   2   1   0 | 5   4   3   2   1   0 | (only lower 6 bits used because base64 is a 6 bit encoding)
     *  |           |           |           |           |           |           |           |           |
     *  |           |           |           |           |           |           |           |           |
     *  |           v           |           v           |           v           |           v           |
     *  | 7   6   5   4   3   2   1   0 | 7   6   5   4   3   2   1   0 | 7   6   5   4   3   2   1   0 |
     *  | decoded 1                     | decoded 2                     | decoded 3                     | <- first 24 bits / 4 hextets / 3 bytes of combined
     *
     *
     *  see https://de.wikipedia.org/wiki/Base64#/media/Datei:Base64-de.png
     */
    uint32_t const combined = (base64_decode_single(c1) << 18) | (base64_decode_single(c2) << 12) | (base64_decode_single(c3) << 6) | base64_decode_single(c4);

    // extract 3 bytes back out of combined
    uint8_t const b1 = (combined >> 16) & 0b1111'1111;
    uint8_t const b2 = (combined >> 8) & 0b1111'1111;
    uint8_t const b3 = combined & 0b1111'1111;
    return {static_cast<std::byte>(b1), static_cast<std::byte>(b2), static_cast<std::byte>(b3)};
}
} // namespace encode_decode_detail


Base64BinaryRepr Base64BinaryRepr::from_encoded(std::string_view const base64encoded) {
    if (base64encoded.empty()) {
        return Base64BinaryRepr{};
    }

    size_t const len = std::count_if(base64encoded.begin(), base64encoded.end(), [](auto const ch) noexcept { return ch != ' '; });
    if (len % 4 != 0) {
        throw std::runtime_error{"xsd:base64Binary parsing error: Invalid number of digits."};
    }

    if (base64encoded.starts_with(' ')) {
        throw std::runtime_error{"xsd:base64Binary parsing error: Stray space at start of encoded string"};
    }

    if (base64encoded.ends_with(' ')) {
        throw std::runtime_error{"xsd:base64Binary parsing error: Stray space at end of encoded string"};
    }

    auto next_char = [&, pos = size_t{0}]() mutable {
        auto ch = ' ';
        auto const old_pos = pos;

        while (ch == ' ') {
            assert(pos < base64encoded.size());
            ch = base64encoded[pos++];
        }

        if (pos - old_pos > 2) {
            throw std::runtime_error{"xsd:base64Binary parsing error: too many consecutive spaces"};
        }

        return ch;
    };

    std::vector<std::byte> buf;
    buf.reserve(base64encoded.size() / 4 * 3);

    for (size_t quad = 0; quad < len / 4 - 1; ++quad) {
        auto const c1 = next_char();
        auto const c2 = next_char();
        auto const c3 = next_char();
        auto const c4 = next_char();

        auto const decoded = encode_decode_detail::base64_decode(c1, c2, c3, c4);
        std::copy(decoded.begin(), decoded.end(), std::back_inserter(buf));
    }

    {
        auto const c1 = next_char();
        auto const c2 = next_char();
        auto const c3 = next_char();
        auto const c4 = next_char();

        auto const c3_pad = c3 == '=';
        auto const c4_pad = c4 == '=';

        if (c3_pad && !c4_pad) {
            throw std::runtime_error{"xsd:binary64 parsing error: Invalid padding"};
        }

        size_t const n = c3_pad + c4_pad;
        auto const decoded = encode_decode_detail::base64_decode(c1, c2, c3_pad ? 'A' : c3, c4_pad ? 'A' : c4);
        std::copy_n(decoded.begin(), 3 - n, std::back_inserter(buf));
    }

    return Base64BinaryRepr{buf};
}

std::string Base64BinaryRepr::to_encoded() const noexcept {
    if (this->n_bytes() == 0) {
        return "";
    }

    std::string buf;
    for (size_t triple = 0; triple < this->n_bytes() / 3; ++triple) {
        // encode all full 3-byte chunks, last chunk that is potentially less than 3 bytes will be handled separately

        auto const b1 = this->byte(triple * 3);
        auto const b2 = this->byte(triple * 3 + 1);
        auto const b3 = this->byte(triple * 3 + 2);

        auto const encoded = encode_decode_detail::base64_encode(b1, b2, b3);
        std::copy(encoded.begin(), encoded.end(), std::back_inserter(buf));
    }

    if (auto const rest = this->n_bytes() % 3; rest != 0) {
        // there is a chunk that is not complete, i.e. < 3 bytes large
        auto const triple = this->n_bytes() / 3;

        auto const b1 = this->byte(triple * 3);
        auto const b2 = triple * 3 + 1 < this->n_bytes() ? this->byte(triple * 3 + 1) : std::byte{0}; // maybe add padding byte for encoding
        auto const b3 = triple * 3 + 2 < this->n_bytes() ? this->byte(triple * 3 + 2) : std::byte{0}; // maybe add padding byte for encoding

        auto const encoded = encode_decode_detail::base64_encode(b1, b2, b3);
        std::copy_n(encoded.begin(), 4 - (3 - rest), std::back_inserter(buf)); // add non-padding / data hextets
        std::fill_n(std::back_inserter(buf), 3 - rest, '='); // add padding hextets to signal that padding bytes were used
    }

    return buf;
}

std::byte Base64BinaryRepr::hextet(size_t const n) const noexcept {
    auto const triple = n / 4 * 3;
    auto const off = (3 - (n % 4)) * 6;

    uint32_t const selected_bytes = static_cast<uint32_t>(this->byte(triple)) << 16
                                    | (triple + 1 < this->n_bytes() ? static_cast<uint32_t>(this->byte(triple + 1)) << 8 : 0)
                                    | (triple + 2 < this->n_bytes() ? static_cast<uint32_t>(this->byte(triple + 2)) : 0);

    return static_cast<std::byte>((selected_bytes >> off) & 0b11'1111);
}

size_t Base64BinaryRepr::n_hextets() const noexcept {
    return 4 * (this->n_bytes() / 3 + (this->n_bytes() % 3 != 0));
}

std::byte Base64BinaryRepr::byte(size_t n) const noexcept {
    return this->bytes[n];
}

size_t Base64BinaryRepr::n_bytes() const noexcept {
    return this->bytes.size();
}

template<>
capabilities::Default<xsd_base64_binary>::cpp_type capabilities::Default<xsd_base64_binary>::from_string(std::string_view s) {
    return Base64BinaryRepr::from_encoded(s);
}

template<>
std::string capabilities::Default<xsd_base64_binary>::to_canonical_string(cpp_type const &value) noexcept {
    return value.to_encoded();
}
#endif

template struct LiteralDatatypeImpl<xsd_base64_binary,
                                    capabilities::FixedId>;


} // namespace rdf4cpp::rdf::datatypes::registry
