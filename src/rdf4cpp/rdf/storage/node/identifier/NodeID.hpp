#ifndef RDF4CPP_NODEID_HPP
#define RDF4CPP_NODEID_HPP

#include <rdf4cpp/rdf/storage/node/identifier/LiteralID.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/LiteralType.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/RDFNodeType.hpp>

#include <rdf4cpp/rdf/datatypes/xsd.hpp>
#include <rdf4cpp/rdf/datatypes/rdf.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>

#include <cassert>
#include <compare>
#include <utility>
#include <string_view>

namespace rdf4cpp::rdf::storage::node::identifier {
/**
 * NodeID is an 48 bit identifier for a Node given a NodeManager. If the Node is a Literal, The 48 bits consist of a LiteralID (42 bits) and a LiteralType (6 bits).
 */
class __attribute__((__packed__)) NodeID {
public:
    static constexpr size_t width = 48;

    static std::pair<NodeID, std::string_view> const default_graph_iri;
    static std::pair<NodeID, std::string_view> const xsd_string_iri;
    static std::pair<NodeID, std::string_view> const rdf_langstring_iri;

    static NodeID const min_bnode_id;
    static NodeID const min_iri_id;
    static NodeID const min_variable_id;
    static LiteralID const min_literal_id;

private:
    struct __attribute__((__packed__)) FullLiteralID {
        LiteralID::underlying_type literal_id : LiteralID::width;
        LiteralType::underlying_type literal_type : LiteralType::width;
    };

    union __attribute__((__packed__)) {
        /**
         * The actual 48 bit identifier.
         */
        uint64_t value_ : width;
        /**
         * Combined Literal ID consisting of LiteralID and literal_type.
         */
        FullLiteralID literal_;
    };

public:
    constexpr NodeID() = default;

    /**
     * Constructs a LiteralID from a single unsigned integer.
     * @param value literal ID. MUST be smaller than 2^48. Bounds are not checked.
     */
    constexpr explicit NodeID(uint64_t value) noexcept : value_(value) { assert(value < (1UL << 48)); }

    /**
     * Constructor to be used for NodeIDs of Literals.
     * @param literal_id the LiteralID
     * @param literal_type the LiteralType
     */
    constexpr NodeID(LiteralID literal_id, LiteralType literal_type) noexcept : literal_{literal_id.value, literal_type.to_underlying()} {}

    /**
     * Get LiteralID. This method does not check if the NodeID actually represents a literal.
     * @return
     */
    [[nodiscard]] constexpr LiteralID literal_id() const noexcept { return LiteralID{literal_.literal_id}; }

    /**
     * Get LiteralType. This method does not check if the NodeID actually represents a literal.
     * @return
     */
    [[nodiscard]] constexpr LiteralType literal_type() const noexcept { return LiteralType::from_underlying(literal_.literal_type); }
    [[nodiscard]] constexpr uint64_t value() const noexcept { return value_; }

    explicit operator uint64_t() const noexcept { return value_; }

    constexpr std::strong_ordering operator<=>(NodeID const &other) const noexcept { return value_ <=> other.value_; }

    constexpr bool operator==(NodeID const &other) const noexcept { return value_ == other.value_; }

    /**
     * Increment value (not literal_id).
     */
    constexpr NodeID &operator++() noexcept {
        ++value_;
        return *this;
    }

    /**
     * Increment value (not literal_id).
     */
    constexpr NodeID operator++(int) noexcept {
        NodeID new_node_id{*this};
        ++value_;
        return new_node_id;
    }

    /**
     * Decrement value (not literal_id).
     */
    constexpr NodeID &operator--() noexcept {
        --value_;
        return *this;
    }

    /**
     * Decrement value (not literal_id).
     */
    constexpr NodeID operator--(int) noexcept {
        NodeID new_node_id{*this};
        --value_;
        return new_node_id;
    }

    /**
     * null NodeID that MUST NOT identify any resource.
     * @return
     */
    [[nodiscard]] constexpr bool null() const noexcept {
        return value_ == 0;
    }
};

static_assert(sizeof(NodeID) == 6);

/**
 * Convert a NodeId for an IRI
 * to a LiteralType.
 *
 * SAFETY: caller must ensure the node id actually refers to an IRI
 *
 * @param id IRI NodeId
 * @return the LiteralType associated with that IRI
 */
constexpr LiteralType iri_node_id_to_literal_type(NodeID const id) noexcept {
    auto const value = id.value();

    return value < datatypes::registry::min_dynamic_datatype_id && value != 0
                   ? LiteralType::from_underlying(static_cast<LiteralType::underlying_type>(value))
                   : LiteralType::other();
}

/**
 * Convert a LiteralType to the corresponding IRI NodeID.
 *
 * @param datatype fixed dataype
 * @return NodeID of the IRI associated with the given datatype
 */
constexpr NodeID literal_type_to_iri_node_id(LiteralType const datatype) {
    assert(datatype.is_fixed());
    return NodeID{datatype.to_underlying()};
}

inline constexpr std::pair<NodeID, std::string_view> NodeID::default_graph_iri{literal_type_to_iri_node_id(datatypes::registry::reserved_datatype_ids[datatypes::registry::default_graph_iri]),
                                                                               datatypes::registry::default_graph_iri};

inline constexpr std::pair<NodeID, std::string_view> NodeID::xsd_string_iri{literal_type_to_iri_node_id(datatypes::xsd::String::fixed_id),
                                                                            datatypes::xsd::String::identifier};

inline constexpr std::pair<NodeID, std::string_view> NodeID::rdf_langstring_iri{literal_type_to_iri_node_id(datatypes::rdf::LangString::fixed_id),
                                                                                datatypes::rdf::LangString::identifier};

inline constexpr NodeID NodeID::min_bnode_id{1};
inline constexpr NodeID NodeID::min_iri_id{datatypes::registry::min_dynamic_datatype_id};
inline constexpr NodeID NodeID::min_variable_id{1};
inline constexpr LiteralID NodeID::min_literal_id{1};

}  // namespace rdf4cpp::rdf::storage::node::identifier

#ifndef DOXYGEN_PARSER
template<>
struct std::hash<rdf4cpp::rdf::storage::node::identifier::NodeID> {
    size_t operator()(rdf4cpp::rdf::storage::node::identifier::NodeID const id) const noexcept {
        return std::hash<uint64_t>{}(id.value());
    }
};

template<typename Policy>
struct dice::hash::dice_hash_overload<Policy, rdf4cpp::rdf::storage::node::identifier::NodeID> {
    static size_t dice_hash(rdf4cpp::rdf::storage::node::identifier::NodeID const id) noexcept {
        return dice_hash_templates<Policy>::dice_hash(id.value());
    }
};
#endif

#endif  //RDF4CPP_NODEID_HPP
