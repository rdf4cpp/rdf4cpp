#include <rdf4cpp/rdf/datatypes/xsd/Base64Binary.hpp>

#include <array>
#include <algorithm>
#include <cstddef>

namespace rdf4cpp::rdf::datatypes::registry {

namespace encode_decode_detail {

// value -> ascii
static constexpr std::array<char, 64> encode_lut{'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
                                                 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
                                                 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
                                                 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
                                                 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2',
                                                 '3', '4', '5', '6', '7', '8', '9', '+', '/'};

// ascii -> value
static constexpr std::array<int8_t, 128> decode_lut{-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                                                    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                                                    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  62,  -1,  -1,  -1,  63,
                                                    52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  -1,  -1,  -1,  -1,  -1,  -1,
                                                    -1,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
                                                    15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  -1,  -1,  -1,  -1,  -1,
                                                    -1,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,
                                                    41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  -1,  -1,  -1,  -1,  -1};

static constexpr std::array<char, 4> base64_encode(std::byte const h1, std::byte const h2, std::byte const h3) noexcept {
    uint32_t const combined = static_cast<uint32_t>(h1) << 16| static_cast<uint32_t>(h2) << 8 | static_cast<uint32_t>(h3);

    return {encode_lut[(combined >> 18) & 0b0011'1111],
            encode_lut[(combined >> 12) & 0b0011'1111],
            encode_lut[(combined >> 6) & 0b0011'1111],
            encode_lut[combined & 0b0011'1111]};
}

static constexpr uint8_t base64_decode_single(char const c) {
    auto const decoded = decode_lut[static_cast<size_t>(c)];
    if (decoded < 0) {
        throw std::runtime_error{"xsd:base64Binary parsing error: invalid digit"};
    }

    return static_cast<uint8_t>(decoded);
}

static constexpr std::array<std::byte, 3> base64_decode(char const c1, char const c2, char const c3, char const c4) {
    uint32_t const combined = (base64_decode_single(c1) << 18) | (base64_decode_single(c2) << 12) | (base64_decode_single(c3) << 6) | base64_decode_single(c4);

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

    size_t const len = std::count_if(base64encoded.begin(), base64encoded.end(), [](auto const ch) { return ch != ' '; });
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
    if (this->empty()) {
        return "";
    }

    std::string buf;
    for (size_t triple = 0; triple < this->size() / 3; ++triple) {
        auto const b1 = (*this)[triple * 3];
        auto const b2 = (*this)[triple * 3 + 1];
        auto const b3 = (*this)[triple * 3 + 2];

        auto const encoded = encode_decode_detail::base64_encode(b1, b2, b3);
        std::copy(encoded.begin(), encoded.end(), std::back_inserter(buf));
    }

    if (auto const rest = this->size() % 3; rest != 0) {
        auto const triple = this->size() / 3;

        auto const b1 = (*this)[triple * 3];
        auto const b2 = triple * 3 + 1 < this->size() ? (*this)[triple * 3 + 1] : std::byte{0};
        auto const b3 = triple * 3 + 2 < this->size() ? (*this)[triple * 3 + 2] : std::byte{0};

        auto const encoded = encode_decode_detail::base64_encode(b1, b2, b3);
        std::copy_n(encoded.begin(), 4 - (3 - rest), std::back_inserter(buf));
        std::fill_n(std::back_inserter(buf), 3 - rest, '=');
    }

    return buf;
}

std::byte Base64BinaryRepr::hextet(size_t const n) const noexcept {
    auto const triple = n / 4 * 3;
    auto const off = (3 - (n % 4)) * 6;

    uint32_t const bytes = static_cast<uint32_t>((*this)[triple]) << 16
                           | (triple + 1 < this->size() ? static_cast<uint32_t>((*this)[triple + 1]) << 8 : 0)
                           | (triple + 2 < this->size() ? static_cast<uint32_t>((*this)[triple + 2]) : 0);

    return static_cast<std::byte>((bytes >> off) & 0b11'1111);
}

size_t Base64BinaryRepr::n_hextets() const noexcept {
    return 4 * (this->size() / 3 + (this->size() % 3 != 0));
}

template<>
capabilities::Default<xsd_base64_binary>::cpp_type capabilities::Default<xsd_base64_binary>::from_string(std::string_view s) {
    return Base64BinaryRepr::from_encoded(s);
}

template<>
std::string capabilities::Default<xsd_base64_binary>::to_string(cpp_type const &value) noexcept {
    return value.to_encoded();
}

template struct LiteralDatatypeImpl<xsd_base64_binary>;


} // namespace rdf4cpp::rdf::datatypes::registry
