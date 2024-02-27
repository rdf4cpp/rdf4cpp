#include "HexBinary.hpp"

namespace rdf4cpp::rdf::datatypes::registry {

#ifndef DOXYGEN_PARSER
namespace encode_decode_detail {

// integer value -> hexadecimal ascii representation (e.g 0 => '0', 10 => 'A')
static constexpr std::array<char, 16> encode_lut{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                                                 'A', 'B', 'C', 'D', 'E', 'F'};

// ascii hexadecimal representation -> integer value (e.g. 'F' => 15, 'a' => 10, 'A' => 10)
// 127 is the error value, meaning the character is invalid for hexadecimal encoding
static constexpr std::array<uint8_t, 128> decode_lut{
        127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,
        127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,
        127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,
          0,    1,    2,    3,    4,    5,    6,    7,    8,    9,  127,  127,  127,  127,  127,  127,
        127,   10,   11,   12,   13,   14,   15,  127,  127,  127,  127,  127,  127,  127,  127,  127,
        127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,
        127,   10,   11,   12,   13,   14,   15,  127,  127,  127,  127,  127,  127,  127,  127,  127,
        127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127,  127};

static uint8_t hex_decode(char const ch) {
    auto const decoded = decode_lut[static_cast<size_t>(ch)];
    if (decoded == 127) {
        throw std::runtime_error{"xsd:binaryHex parsing error: invalid digit"};
    }

    return decoded;
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

    // Leading zeroes are irrelevant, find first non-zero digit. Decoding will ignore leading zeros.
    auto const start = hex_encoded.find_first_not_of('0');
    if (start == std::string_view::npos) {
        return HexBinaryRepr{};
    }

    std::vector<std::byte> buf;
    buf.reserve((hex_encoded.size() - start) / 2);

    // rest == 1 if there is an incomplete byte (i.e. a byte not represented by two digits) at the front
    // needs separate handling if there is
    auto const rest = static_cast<ssize_t>((hex_encoded.size() - start) % 2);

    for (ssize_t ix = static_cast<ssize_t>(hex_encoded.size()) - 1; ix >= static_cast<ssize_t>(start + rest); ix -= 2) {
        // loop (backwards) over all full bytes, i.e. bytes that are represented by two hex digits
        // also, ignore leading zero digits
        // example: in 00A98B1 this loops over B1, 98

        auto const higher = encode_decode_detail::hex_decode(hex_encoded[ix - 1]);
        auto const lower = encode_decode_detail::hex_decode(hex_encoded[ix]);

        buf.push_back(static_cast<std::byte>((higher << 4) | lower));
    }

    if (rest > 0) {
        // handle last byte that was not represented by two digits (if there is one)
        // example: in 00A98B1 this decodes A
        auto const missing = encode_decode_detail::hex_decode(hex_encoded[start]);
        buf.push_back(static_cast<std::byte>(missing));
    }

    return HexBinaryRepr{buf};
}

std::string HexBinaryRepr::to_encoded() const noexcept {
    return writer::StringWriter::oneshot([this](auto &w) noexcept {
        return this->serialize(w);
    });
}

bool HexBinaryRepr::serialize(std::span<std::byte const> bytes, void *buffer, writer::Cursor *cursor, writer::FlushFunc flush) noexcept {
    if (bytes.empty()) {
        return writer::write_str("0", buffer, cursor, flush);
    }

    for (ssize_t ix = static_cast<ssize_t>(bytes.size()) - 1; ix >= 0; --ix) {
        auto const byte = bytes[ix];
        auto const lower = static_cast<uint8_t>(byte) & 0b1111;
        auto const higher = (static_cast<uint8_t>(byte) >> 4) & 0b1111;

        std::array<char, 2> const chars{encode_decode_detail::hex_encode(higher), encode_decode_detail::hex_encode(lower)};

        if (!writer::write_str(std::string_view{chars.data(), chars.size()}, buffer, cursor, flush)) {
            return false;
        }
    }

    return true;
}

bool HexBinaryRepr::serialize(void *buffer, writer::Cursor *cursor, writer::FlushFunc flush) const noexcept {
    return serialize(this->bytes, buffer, cursor, flush);
}

std::byte HexBinaryRepr::half_octet(size_t const n) const noexcept {
    auto const ix = n / 2;
    auto const off = n % 2;
    return (this->byte(ix) >> (4 * off)) & std::byte{0b1111};
}

size_t HexBinaryRepr::n_half_octets() const noexcept {
    return this->n_bytes() * 2;
}

std::byte HexBinaryRepr::byte(size_t n) const noexcept {
    return this->bytes[n];
}

size_t HexBinaryRepr::n_bytes() const noexcept {
    return this->bytes.size();
}

template<>
capabilities::Default<xsd_hex_binary>::cpp_type capabilities::Default<xsd_hex_binary>::from_string(std::string_view const s) {
    return HexBinaryRepr::from_encoded(s);
}

template<>
bool capabilities::Default<xsd_hex_binary>::serialize_canonical_string(cpp_type const &value, void *buffer, writer::Cursor *cursor, writer::FlushFunc flush) noexcept {
    return value.serialize(buffer, cursor, flush);
}
#endif

template struct LiteralDatatypeImpl<xsd_hex_binary,
                                    capabilities::FixedId>;

} // namespace rdf4cpp::rdf::datatypes::registry
