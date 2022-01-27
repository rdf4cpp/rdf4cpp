#ifndef RDF4CPP_NODEID_HPP
#define RDF4CPP_NODEID_HPP

#include <rdf4cpp/rdf/storage/node/identifier/RDFNodeType.hpp>

#include <bitset>
#include <cassert>
#include <compare>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace rdf4cpp::rdf::storage::node::identifier {

enum class LiteralType : uint16_t {
    // TODO: support different literal types
    STRING = 0,
    INT,
    FLOAT
    // ...

};

struct __attribute__((__packed__)) LiteralID {
    // TODO: inline literal types < 42 bits into the literalID
    uint64_t value : 42;

    LiteralID() = default;

    explicit LiteralID(uint64_t value) noexcept;

    auto operator<=>(LiteralID const &) const noexcept = default;

    bool operator==(LiteralID const &) const noexcept = default;

    LiteralID &operator++() noexcept;

    LiteralID operator++(int) noexcept;
};

struct __attribute__((__packed__)) NodeIDValue {
    union __attribute__((__packed__)) {
        uint64_t value : 48;
        struct __attribute__((__packed__)) {
            uint64_t literal_id_ : 42;
            LiteralType literal_type_ : 6;
        } literal_;
    };

    NodeIDValue() = default;

    explicit NodeIDValue(uint64_t value) noexcept;

    NodeIDValue(LiteralID literalId, LiteralType literalType) noexcept;

    [[nodiscard]] LiteralID literal_id() const noexcept;

    [[nodiscard]] LiteralType literal_type() const noexcept;

    std::strong_ordering operator<=>(NodeIDValue const &other) const noexcept;

    bool operator==(NodeIDValue const &other) const noexcept;

    NodeIDValue &operator++() noexcept;

    NodeIDValue operator++(int) noexcept;
};

struct __attribute__((__packed__)) NodeStorageID {
    uint16_t value : 10;

    auto operator<=>(NodeStorageID const &) const noexcept = default;
    bool operator==(NodeStorageID const &) const noexcept = default;
};

class NodeID {
public:
    static std::pair<NodeIDValue, std::string> const default_graph_iri;
    static std::pair<NodeIDValue, std::string> const xsd_string_iri;
    static std::pair<NodeIDValue, std::string> const rdf_langstring_iri;

    static std::vector<std::pair<NodeIDValue, std::string>> const predefined_iris;
    static NodeIDValue const min_iri_id;
    // TODO: that might be specified further for literals
    static LiteralID const min_literal_id;
    static NodeIDValue const min_bnode_id;
    static NodeIDValue const min_variable_id;

private:
    union __attribute__((__packed__)) {
        uint64_t raw_{};
        struct __attribute__((__packed__)) {
            NodeIDValue node_id_;
            RDFNodeType type_ : 2;
            uint16_t manager_id_ : 10;
            uint8_t free_tagging_bits : 4;
        } fields_;
    };

public:
    NodeID() noexcept;

    explicit NodeID(size_t raw) noexcept;

    NodeID(NodeStorageID manager_id, RDFNodeType type, NodeIDValue node_id, uint8_t tagging_bits = uint8_t(0)) noexcept;

    NodeID(NodeStorageID manager_id, RDFNodeType type, LiteralID literal_id, LiteralType literal_type, uint8_t tagging_bits = uint8_t(0)) noexcept;

    [[nodiscard]] NodeStorageID manager_id() const noexcept;

    [[nodiscard]] uint8_t free_tagging_bits() const noexcept;

    void free_tagging_bits(uint8_t new_value);

    [[nodiscard]] const NodeIDValue &node_id() const noexcept;

    [[nodiscard]] uint64_t raw() const noexcept;

    [[nodiscard]] RDFNodeType type() const noexcept;

    [[nodiscard]] bool is_type(RDFNodeType type_) const noexcept;

    [[nodiscard]] bool empty() const noexcept;

    // TODO: support basic xsd literal datatypes for comparison
    std::partial_ordering operator<=>(NodeID const &other) const noexcept;

    bool operator==(NodeID const &other) const noexcept;

    // TODO: operator std::string and operator<< overload
    [[nodiscard]] std::string as_string() const noexcept;
};

static_assert(sizeof(NodeID) == sizeof(uint64_t));


}  // namespace rdf4cpp::rdf::storage::node::identifier

#endif  //RDF4CPP_NODEID_HPP
