#ifndef RDF4CPP_XSD_HEXBINARY_HPP
#define RDF4CPP_XSD_HEXBINARY_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

#include <cstddef>
#include <vector>

namespace rdf4cpp::rdf::datatypes::registry {

/**
 * Represents a decoded xsd:hexBinary value as a byte sequence
 * where each byte stores 2 half-octets.
 *
 * The bytes are stored in order least significant to most significant (i.e. index 0 is least significant).
 */
struct HexBinaryRepr : std::vector<std::byte> {
    [[nodiscard]] static HexBinaryRepr from_encoded(std::string_view hex_encoded);
    [[nodiscard]] std::string to_encoded() const noexcept;

    /**
     * @param n index of half-octet / hex digit to extract
     * @return the n-th half-octet of this byte sequence
     * @note the least significant half-octet has index 0
     */
    [[nodiscard]] constexpr std::byte half_octet(size_t const n) const noexcept {
        auto const ix = n / 2;
        auto const off = n % 2;
        return static_cast<std::byte>((static_cast<uint8_t>((*this)[ix]) >> (4 * off)) & 0b1111);
    }

    /**
     * @return the number of half-octets / hex digits represented by this byte sequence
     */
    [[nodiscard]] constexpr size_t n_half_octets() const noexcept {
        return this->size() * 2;
    }

    std::strong_ordering operator<=>(HexBinaryRepr const &) const noexcept = default;
};


inline constexpr util::ConstexprString xsd_hex_binary{"http://www.w3.org/2001/XMLSchema#hexBinary"};

template<>
struct DatatypeMapping<xsd_hex_binary> {
    using cpp_datatype = HexBinaryRepr;
};

template<>
capabilities::Default<xsd_hex_binary>::cpp_type capabilities::Default<xsd_hex_binary>::from_string(std::string_view s);

template<>
std::string capabilities::Default<xsd_hex_binary>::to_string(cpp_type const &value) noexcept;

extern template struct LiteralDatatypeImpl<xsd_hex_binary>;

} // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::xsd {

struct HexBinary : registry::LiteralDatatypeImpl<registry::xsd_hex_binary> {};

} // namespace rdf4cpp::rdf::datatypes::xsd


namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline xsd::HexBinary const xsd_hex_binary_instance;

} // namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail

#endif // RDF4CPP_XSD_HEXBINARY_HPP
