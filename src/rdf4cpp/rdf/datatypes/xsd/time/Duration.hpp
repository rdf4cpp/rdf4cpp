#ifndef RDF4CPP_DURATION_HPP
#define RDF4CPP_DURATION_HPP

#include <chrono>

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/util/Timezone.hpp>
#include <dice/hash.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

#ifndef DOXYGEN_PARSER
template<>
struct DatatypeMapping<xsd_duration> {
    using cpp_datatype = std::pair<std::chrono::months, std::chrono::milliseconds>;
};


template<>
capabilities::Default<xsd_duration>::cpp_type capabilities::Default<xsd_duration>::from_string(std::string_view s);

template<>
std::string capabilities::Default<xsd_duration>::to_canonical_string(const cpp_type &value) noexcept;

template<>
std::optional<storage::node::identifier::LiteralID> capabilities::Inlineable<xsd_duration>::try_into_inlined(cpp_type const &value) noexcept;

template<>
capabilities::Inlineable<xsd_duration>::cpp_type capabilities::Inlineable<xsd_duration>::from_inlined(storage::node::identifier::LiteralID inlined) noexcept;

template<>
std::partial_ordering capabilities::Comparable<xsd_duration>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept;
#endif

extern template struct LiteralDatatypeImpl<xsd_duration,
                                           capabilities::Comparable,
                                           capabilities::FixedId,
                                           capabilities::Inlineable>;

}  // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {

struct Duration : registry::LiteralDatatypeImpl<registry::xsd_duration,
                                                registry::capabilities::Comparable,
                                                registry::capabilities::FixedId,
                                                registry::capabilities::Inlineable> {};

}  // namespace rdf4cpp::rdf::datatypes::xsd


namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline xsd::Duration const xsd_Duration_instance;

}  // namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail

template<typename Policy, typename A, typename B>
struct dice::hash::dice_hash_overload<Policy, std::chrono::duration<A, B>> {
    static size_t dice_hash(std::chrono::duration<A, B> const &x) noexcept {
        auto m = x.count();
        return dice::hash::dice_hash_templates<Policy>::dice_hash(m);
    }
};

#endif  //RDF4CPP_DURATION_HPP
