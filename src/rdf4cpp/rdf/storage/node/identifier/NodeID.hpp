#ifndef RDF4CPP_NODEID_HPP
#define RDF4CPP_NODEID_HPP

#include <rdf4cpp/rdf/storage/node/identifier/LiteralID.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/LiteralType.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/RDFNodeType.hpp>

#include <rdf4cpp/rdf/datatypes/xsd.hpp>
#include <rdf4cpp/rdf/storage/util/StaticVec.hpp>

#include <array>
#include <cassert>
#include <compare>
#include <tuple>
#include <vector>
#include <string_view>

namespace rdf4cpp::rdf::storage::node::identifier {
/**
 * NodeID is an 48 bit identifier for a Node given a NodeManager. If the Node is a Literal, The 48 bits consist of a LiteralID (42 bits) and a LiteralType (6 bits).
 */
class __attribute__((__packed__)) NodeID {
public:
    static std::pair<NodeID, std::string_view> const default_graph_iri;
    static std::pair<NodeID, std::string_view> const xsd_string_iri;
    static std::pair<NodeID, std::string_view> const rdf_langstring_iri;

    static util::StaticVec<std::pair<NodeID, std::string_view>, 64> const predefined_iris;

    static NodeID const min_bnode_id;
    static NodeID const min_iri_id;
    static NodeID const min_variable_id;

    // TODO: that might be specified further for specific LiteralTypes
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
        uint64_t value_ : 48;
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
    //    void value(uint64_t val) { assert(val < (1UL << 48)); value_ = val; }

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

inline constexpr std::pair<NodeID, std::string_view> NodeID::default_graph_iri{NodeID{1}, ""};
inline constexpr std::pair<NodeID, std::string_view> NodeID::xsd_string_iri{datatypes::xsd::String::fixed_id, datatypes::xsd::String::identifier};
inline constexpr std::pair<NodeID, std::string_view> NodeID::rdf_langstring_iri{NodeID{3}, "http://www.w3.org/1999/02/22-rdf-syntax-ns#langString"};

inline constexpr util::StaticVec<std::pair<NodeID, std::string_view>, 64> NodeID::predefined_iris{
        default_graph_iri,
        xsd_string_iri,
        rdf_langstring_iri,
        {NodeID{datatypes::xsd::Int::fixed_id}, datatypes::xsd::Int::identifier},
        {NodeID{datatypes::xsd::Integer::fixed_id}, datatypes::xsd::Integer::identifier}}; // todo: maybe find better way


inline constexpr NodeID NodeID::min_bnode_id{1};
inline constexpr NodeID NodeID::min_iri_id{NodeID::predefined_iris.max_size() + 1};
inline constexpr NodeID NodeID::min_variable_id{1};
inline constexpr LiteralID NodeID::min_literal_id{1};


}  // namespace rdf4cpp::rdf::storage::node::identifier


#endif  //RDF4CPP_NODEID_HPP
