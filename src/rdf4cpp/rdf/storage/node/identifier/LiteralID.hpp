#ifndef RDF4CPP_LITERALID_HPP
#define RDF4CPP_LITERALID_HPP

#include <cassert>
#include <compare>
#include <cstdint>

namespace rdf4cpp::rdf::storage::node::identifier {
/**
 * <p>An LiteralID identifies a Literal of a given type (e.g. xsd:integer, xsd:string, etc.) within a given NodeStorage.</p>
 * <p>LiteralIDs are available in the range [0,(2^42-1)].</P>
 */
struct __attribute__((__packed__)) LiteralID {
    static constexpr std::size_t width = 42;
    using underlying_type = uint64_t;
    underlying_type value : width{};

    constexpr LiteralID() = default;

    /**
     * Constructor
     * @param value literal ID. MUST be smaller than 2^42. Bounds are not checked.
     */
    constexpr explicit LiteralID(uint64_t value) noexcept : value(value) { assert(value < (1UL << 42)); }

    constexpr std::strong_ordering operator<=>(LiteralID const &) const noexcept = default;

    constexpr LiteralID &operator++() noexcept {
        ++value;
        return *this;
    }

    [[nodiscard]] constexpr underlying_type to_underlying() const noexcept {
        return value;
    }

    explicit operator underlying_type() const noexcept {
        return value;
    }

    constexpr LiteralID operator++(int) noexcept {
        LiteralID new_literal_id{*this};
        ++value;
        return new_literal_id;
    }

    constexpr LiteralID &operator--() noexcept {
        --value;
        return *this;
    }

    constexpr LiteralID operator--(int) noexcept {
        LiteralID new_literal_id{*this};
        --value;
        return new_literal_id;
    }
};
}  // namespace rdf4cpp::rdf::storage::node::identifier
#endif  //RDF4CPP_LITERALID_HPP
