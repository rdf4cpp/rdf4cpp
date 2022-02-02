#ifndef RDF4CPP_NODESTORAGEID_HPP
#define RDF4CPP_NODESTORAGEID_HPP

#include <compare>
#include <cstdint>

namespace rdf4cpp::rdf::storage::node::identifier {
/**
 * <p>An NodeStorageID identifies a Literal of a given type (e.g. xsd:integer, xsd:string, etc.) within a given NodeStorage.</p>
 * <p>NodeStorageIDs are available in the range [0,(2^10-1)].</P>
 */
struct __attribute__((__packed__)) NodeStorageID {
    static constexpr std::size_t width = 10;
    using underlying_type = uint16_t;
    underlying_type value : width;

    constexpr NodeStorageID() = default;

    /**
     * Constructor
     * @param value NodeStorageID. MUST be smaller than 2^10. Bounds are not checked.
     */
    constexpr explicit NodeStorageID(uint16_t value) noexcept : value(value) {}

    constexpr auto operator<=>(NodeStorageID const &) const noexcept = default;

    constexpr bool operator==(NodeStorageID const &) const noexcept = default;

    constexpr NodeStorageID &operator++() noexcept {
        ++value;
        return *this;
    }

    constexpr NodeStorageID operator++(int) noexcept {
        NodeStorageID new_literal_id{*this};
        ++value;
        return new_literal_id;
    }

    constexpr NodeStorageID &operator--() noexcept {
        --value;
        return *this;
    }

    constexpr NodeStorageID operator--(int) noexcept {
        NodeStorageID new_literal_id{*this};
        --value;
        return new_literal_id;
    }
};
}  // namespace rdf4cpp::rdf::storage::node::identifier

#endif  //RDF4CPP_NODESTORAGEID_HPP
