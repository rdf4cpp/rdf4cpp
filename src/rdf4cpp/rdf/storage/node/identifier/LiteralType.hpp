#ifndef RDF4CPP_LITERALTYPE_HPP
#define RDF4CPP_LITERALTYPE_HPP

#include <cstdint>
#include <cstddef>
#include <cassert>


namespace rdf4cpp::rdf::storage::node::identifier {

/**
 * <p>A literal type specifies the type of a literal. Types, which are not available within this enum class MUST be specified as LiteralType::OTHER.</p>
 * <p>The purpose of this type is to provide a short-cut for common types like xsd:string or xsd:float when used within NodeID or NodeBackendHandle.
 * If a LiteralType instance is part of an identifier or view for a Literal stored in NodeStorage, it must not contradict the type information stored therein. </p>
 */
struct __attribute__((__packed__)) LiteralType {
    using underlying_type = uint8_t;
    static constexpr size_t width = 6;
    static constexpr size_t num_fixed_types = 63;

private:
    /*union __attribute__((__packed__)) {
        uint8_t underlying: 6;

        struct __attribute__((__packed__)) {
            bool numeric_tagging_bit: 1;
            uint8_t type_id: 5;
        } fields;
    };*/
    uint8_t underlying: 6;

public:
    static constexpr LiteralType other() noexcept {
        LiteralType ret;
        ret.underlying = 0;
        return ret;
    }

    static constexpr LiteralType from_underlying(underlying_type const underlying) noexcept {
        assert((underlying & 0b1100'0000) == 0);
        LiteralType ret;
        ret.underlying = underlying;

        return ret;
    }

    /*static constexpr LiteralType from_parts(bool const is_numeric, uint8_t const type_id) noexcept {
        assert((type_id & 0b1110'0000) == 0);
        LiteralType ret;
        ret.fields = { is_numeric, type_id };

        return ret;
    }*/

    static constexpr LiteralType from_iri_node_id(uint64_t const iri_id) noexcept {
        if (iri_id <= num_fixed_types && iri_id != LiteralType::other().to_underlying()) {
            return LiteralType::from_underlying(static_cast<underlying_type>(iri_id));
        } else {
            return LiteralType::other();
        }
    }

    [[nodiscard]] constexpr bool is_fixed() const noexcept {
        return *this != LiteralType::other();
    }

    /*[[nodiscard]] constexpr bool is_numeric() const noexcept {
        return this->fields.numeric_tagging_bit;
    }

    [[nodiscard]] constexpr uint8_t type_id() const noexcept {
        return this->fields.type_id;
    }*/

    [[nodiscard]] constexpr underlying_type to_underlying() const noexcept {
        return this->underlying;
    }

    constexpr bool operator==(LiteralType const &other) const noexcept {
        return this->to_underlying() == other.to_underlying();
    }
};

//enum class /*__attribute__((__packed__)) */ LiteralType : uint8_t {
//    OTHER = 0,
//    // ...
//    // TODO: add XSD types and langString
//
//};

}  // namespace rdf4cpp::rdf::storage::node::identifier

#endif  //RDF4CPP_LITERALTYPE_HPP
