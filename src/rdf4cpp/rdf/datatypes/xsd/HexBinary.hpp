#ifndef RDF4CPP_XSD_HEXBINARY_HPP
#define RDF4CPP_XSD_HEXBINARY_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>

#include <dice/hash.hpp>

#include <cstddef>
#include <vector>

namespace rdf4cpp::rdf::datatypes::registry {

/**
 * Represents a decoded xsd:hexBinary value as a byte sequence
 * where each byte stores 2 half-octets.
 *
 * The bytes are stored in order least significant to most significant (i.e. index 0 is least significant).
 */
struct HexBinaryRepr {
    std::vector<std::byte> bytes;

    /**
     * Constructs a HexBinaryRepr from an hexadecimal-encoded string
     * @param hex_encoded encoded hexadecimal string
     * @return decoded hexadecimal value
     */
    [[nodiscard]] static HexBinaryRepr from_encoded(std::string_view hex_encoded);

    /**
     * Encodes this hexadecimal value into its string representation
     * @return encoded string representation
     */
    [[nodiscard]] std::string to_encoded() const noexcept;

    bool serialize(void *buffer, writer::Cursor *cursor, writer::FlushFunc flush) const noexcept;

    template<writer::BufWriter W>
    bool serialize(W &w) const noexcept {
        return this->serialize(&w.buffer(), &w.cursor(), &W::flush);
    }

    /**
     * @param n index of half-octet / hex digit to extract
     * @return the n-th half-octet of this byte sequence
     * @note the least significant half-octet has index 0
     * @warning the provided index n must be less than n_half_octets() otherwise the behaviour is undefined
     */
    [[nodiscard]] std::byte half_octet(size_t n) const noexcept;

    /**
     * @return the number of half-octets / hex digits represented by this byte sequence
     */
    [[nodiscard]] size_t n_half_octets() const noexcept;

    /**
     * @param n index of the byte to extract
     * @return the n-th byte of this sequence
     * @note the bytes are stored in order least significant to most significant (i.e. index 0 is least significant).
     * @warning the provided index n must be less than n_bytes() otherwise the behaviour is undefined
     */
    [[nodiscard]] std::byte byte(size_t n) const noexcept;

    /**
     * @return the number of bytes in this encoded hexadecimal value
     */
    [[nodiscard]] size_t n_bytes() const noexcept;

    std::strong_ordering operator<=>(HexBinaryRepr const &) const noexcept = default;
};

#ifndef DOXYGEN_PARSER
template<>
struct DatatypeMapping<xsd_hex_binary> {
    using cpp_datatype = HexBinaryRepr;
};

template<>
capabilities::Default<xsd_hex_binary>::cpp_type capabilities::Default<xsd_hex_binary>::from_string(std::string_view s);

template<>
bool capabilities::Default<xsd_hex_binary>::serialize_canonical_string(cpp_type const &value, void *buffer, writer::Cursor *cursor, writer::FlushFunc flush) noexcept;
#endif

extern template struct LiteralDatatypeImpl<xsd_hex_binary,
                                           capabilities::FixedId>;

} // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {

struct HexBinary : registry::LiteralDatatypeImpl<registry::xsd_hex_binary,
                                                 registry::capabilities::FixedId> {};

} // namespace rdf4cpp::rdf::datatypes::xsd


namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline xsd::HexBinary const xsd_hex_binary_instance;

} // namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail

#ifndef DOXYGEN_PARSER
template<typename Policy>
struct dice::hash::dice_hash_overload<Policy, rdf4cpp::rdf::datatypes::registry::HexBinaryRepr> {
    static size_t dice_hash(rdf4cpp::rdf::datatypes::registry::HexBinaryRepr const &x) noexcept {
        return Policy::hash_bytes(reinterpret_cast<char const *>(x.bytes.data()), x.bytes.size());
    }
};

template<>
struct std::hash<rdf4cpp::rdf::datatypes::registry::HexBinaryRepr> {
    size_t operator()(rdf4cpp::rdf::datatypes::registry::HexBinaryRepr const &value) const noexcept {
        return dice::hash::dice_hash_templates<dice::hash::Policies::wyhash>::dice_hash(value);
    }
};
#endif

#endif // RDF4CPP_XSD_HEXBINARY_HPP
