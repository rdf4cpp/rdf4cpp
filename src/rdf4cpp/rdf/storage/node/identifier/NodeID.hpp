#ifndef RDF4CPP_NODEID_HPP
#define RDF4CPP_NODEID_HPP

#include <rdf4cpp/rdf/storage/node/identifier/LiteralID.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/LiteralType.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/RDFNodeType.hpp>

#include <cassert>
#include <compare>
#include <tuple>
#include <vector>

namespace rdf4cpp::rdf::storage::node::identifier {
/**
 * NodeID is an 48 bit identifier for a Node given a NodeManager. If the Node is a Literal, The 48 bits consist of a LiteralID (42 bits) and a LiteralType (6 bits).
 */
class __attribute__((__packed__)) NodeID {
public:
    static std::pair<NodeID, std::string> const default_graph_iri;
    static std::pair<NodeID, std::string> const xsd_string_iri;
    static std::pair<NodeID, std::string> const rdf_langstring_iri;

    static std::vector<std::pair<NodeID, std::string>> const predefined_iris;
    static NodeID const min_iri_id;
    // TODO: that might be specified further for specific LiteralTypes
    static LiteralID const min_literal_id;
    static NodeID const min_bnode_id;
    static NodeID const min_variable_id;

private:
    struct __attribute__((__packed__)) FullLiteralID {
        LiteralID::underlying_type literal_id : LiteralID::width;
        LiteralType literal_type : 6;
    };

    union __attribute__((__packed__)) {
        /**
         * The actual 48 bit identifier.
         */
        uint64_t value_ : 48;
        FullLiteralID literal_;
    };

public:
    constexpr NodeID() = default;

    /**
     * Constructor
     * @param value literal ID. MUST be smaller than 2^48. Bounds are not checked.
     */
    constexpr explicit NodeID(uint64_t value) noexcept : value_(value) { assert(value < (1UL << 48)); }

    constexpr NodeID(LiteralID literal_id, LiteralType literal_type) noexcept : literal_{literal_id.value, literal_type} {}

    [[nodiscard]] constexpr LiteralID literal_id() const noexcept { return LiteralID{literal_.literal_id}; }

    [[nodiscard]] constexpr LiteralType literal_type() const noexcept { return literal_.literal_type; }
    [[nodiscard]] constexpr uint64_t value() const noexcept { return value_; }
    //    void value(uint64_t val) { assert(val < (1UL << 48)); value_ = val; }

    constexpr std::partial_ordering operator<=>(NodeID const &other) const noexcept { return value_ <=> other.value_; }

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
        // TODO: make sure null() NodeIDs are nowhere used to identify valid nodes.
        return value_ == 0;
    }
};

static_assert(sizeof(NodeID) == 6);

struct TypedNodeID : public NodeID {
private:
    RDFNodeType node_type_;

public:
    constexpr TypedNodeID() : NodeID(), node_type_{} {}

    /**
     * Constructor
     * @param value literal ID. MUST be smaller than 2^48. Bounds are not checked.
     */
    constexpr explicit TypedNodeID(uint64_t value, RDFNodeType node_type) noexcept : NodeID(value), node_type_(node_type) {}


    TypedNodeID(LiteralID literalId, LiteralType literalType) noexcept : NodeID(literalId, literalType), node_type_(RDFNodeType::Literal) {}

    constexpr RDFNodeType node_type() const noexcept { return node_type_; }
};

}  // namespace rdf4cpp::rdf::storage::node::identifier


#endif  //RDF4CPP_NODEID_HPP
