#include <rdf4cpp/rdf/datatypes/xsd/HexBinary.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

namespace encode_decode_detail {

static constexpr std::array<char, 16> encode_lut{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                                                 'A', 'B', 'C', 'D', 'E', 'F'};

static constexpr std::array<int8_t, 128> decode_lut{
        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
        0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  -1,  -1,  -1,  -1,  -1,  -1,
        -1,  10,  11,  12,  13,  14,  15,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
        -1,  10,  11,  12,  13,  14,  15,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1};

static uint8_t hex_decode(char const ch) {
    auto const decoded = decode_lut[static_cast<size_t>(ch)];
    if (decoded < 0) {
        throw std::runtime_error{"xsd:binaryHex parsing error: invalid digit"};
    }

    return static_cast<uint8_t>(decoded);
}

static char hex_encode(uint8_t const half_octet) noexcept {
    assert(half_octet <= 15);
    return encode_lut[half_octet];
}

} // namespace encode_decode_detail

HexBinaryRepr HexBinaryRepr::from_encoded(std::string_view const hex_encoded) {
    if (hex_encoded.empty()) {
        return HexBinaryRepr{};
    }

    auto const start = hex_encoded.find_first_not_of('0');
    if (start == std::string_view::npos) {
        return HexBinaryRepr{};
    }

    std::vector<std::byte> buf;
    buf.reserve((hex_encoded.size() - start) / 2);

    auto const rest = static_cast<ssize_t>((hex_encoded.size() - start) % 2);

    for (ssize_t ix = static_cast<ssize_t>(hex_encoded.size()) - 1; ix >= static_cast<ssize_t>(start + rest); ix -= 2) {
        auto const higher = encode_decode_detail::hex_decode(hex_encoded[ix - 1]);
        auto const lower = encode_decode_detail::hex_decode(hex_encoded[ix]);

        buf.push_back(static_cast<std::byte>((higher << 4) | lower));
    }

    if (rest > 0) {
        auto const missing = encode_decode_detail::hex_decode(hex_encoded[start]);
        if (missing != 0) {
            buf.push_back(static_cast<std::byte>(missing));
        }
    }

    return HexBinaryRepr{buf};
}

std::string HexBinaryRepr::to_encoded() const noexcept {
    if (this->empty()) {
        return "0";
    }

    std::string buf;
    for (ssize_t ix = static_cast<ssize_t>(this->size()) - 1; ix >= 0; --ix) {
        auto const byte = (*this)[ix];
        auto const lower = static_cast<uint8_t>(byte) & 0b1111;
        auto const higher = (static_cast<uint8_t>(byte) >> 4) & 0b1111;

        buf.push_back(encode_decode_detail::hex_encode(higher));
        buf.push_back(encode_decode_detail::hex_encode(lower));
    }

    return buf;
}

std::byte HexBinaryRepr::half_octet(size_t const n) const noexcept {
    auto const ix = n / 2;
    auto const off = n % 2;
    return ((*this)[ix] >> (4 * off)) & std::byte{0b1111};
}

size_t HexBinaryRepr::n_half_octets() const noexcept {
    return this->size() * 2;
}

template<>
capabilities::Default<xsd_hex_binary>::cpp_type capabilities::Default<xsd_hex_binary>::from_string(std::string_view const s) {
    return HexBinaryRepr::from_encoded(s);
}

template<>
std::string capabilities::Default<xsd_hex_binary>::to_string(cpp_type const &value) noexcept {
    return value.to_encoded();
}

template struct LiteralDatatypeImpl<xsd_hex_binary>;

} // namespace rdf4cpp::rdf::datatypes::registry
