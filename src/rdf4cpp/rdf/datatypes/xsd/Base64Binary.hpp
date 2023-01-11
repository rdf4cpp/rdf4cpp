#ifndef RDF4CPP_XSD_BASE64BINARY_HPP
#define RDF4CPP_XSD_BASE64BINARY_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

#include <cstddef>
#include <vector>

namespace rdf4cpp::rdf::datatypes::registry {

struct Base64BinaryRepr : std::vector<std::byte> {
    [[nodiscard]] static Base64BinaryRepr from_encoded(std::string_view base64encoded);
    [[nodiscard]] std::string to_encoded() const noexcept;

    [[nodiscard]] constexpr std::byte hextet(size_t const n) const noexcept {
        return {};
    }

    [[nodiscard]] constexpr size_t n_hextets() const noexcept {
        return 4 * (this->size() / 3 + (this->size() % 3 != 0));
    }

    std::strong_ordering operator<=>(Base64BinaryRepr const &) const noexcept = default;
};


inline constexpr util::ConstexprString xsd_base64_binary{"http://www.w3.org/2001/XMLSchema#base64Binary"};

template<>
struct DatatypeMapping<xsd_base64_binary> {
    using cpp_datatype = Base64BinaryRepr;
};

template<>
capabilities::Default<xsd_base64_binary>::cpp_type capabilities::Default<xsd_base64_binary>::from_string(std::string_view s);

template<>
std::string capabilities::Default<xsd_base64_binary>::to_string(cpp_type const &value) noexcept;

extern template struct LiteralDatatypeImpl<xsd_base64_binary>;

} // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {

struct Base64Binary : registry::LiteralDatatypeImpl<registry::xsd_base64_binary> {};

} // namespace rdf4cpp::rdf::datatypes::xsd

namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline xsd::Base64Binary const xsd_base64_binary_instance;

} // namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail

#endif //RDF4CPP_XSD_BASE64BINARY_HPP
