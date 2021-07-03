#ifndef RDF4CPP_NODEID_HPP
#define RDF4CPP_NODEID_HPP

#include <rdf4cpp/rdf/storage/node/RDFNodeType.hpp>

#include <bitset>
#include <cassert>
#include <compare>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace rdf4cpp::rdf::storage::node {

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

    explicit LiteralID(uint64_t value);

    auto operator<=>(const LiteralID &) const = default;

    bool operator==(const LiteralID &) const = default;

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

    NodeIDValue(uint64_t value);
    NodeIDValue(LiteralID literalId, LiteralType literalType);


    [[nodiscard]] LiteralID literal_id() const;

    [[nodiscard]] LiteralType literal_type() const;

    std::strong_ordering operator<=>(const NodeIDValue &other) const;

    bool operator==(const NodeIDValue &other) const;

    NodeIDValue &operator++() noexcept;

    NodeIDValue operator++(int) noexcept;
};

struct __attribute__((__packed__)) NodeStorageID {
    uint16_t value : 10;

    auto operator<=>(const NodeStorageID &) const = default;
    bool operator==(const NodeStorageID &) const = default;
};

class NodeID {
public:
    static const std::pair<NodeIDValue, std::string> default_graph_iri;
    static const std::pair<NodeIDValue, std::string> xsd_string_iri;
    static const std::pair<NodeIDValue, std::string> rdf_langstring_iri;

    static const std::vector<std::pair<NodeIDValue, std::string>> predefined_iris;
    static const NodeIDValue min_iri_id;
    // TODO: that might be specified further for literals
    static const LiteralID min_literal_id;
    static const NodeIDValue min_bnode_id;
    static const NodeIDValue min_variable_id;

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
    NodeID();
    explicit NodeID(size_t raw);

    NodeID(NodeStorageID manager_id, RDFNodeType type, NodeIDValue node_id, uint8_t tagging_bits = uint8_t(0));

    NodeID(NodeStorageID manager_id, RDFNodeType type, LiteralID literal_id, LiteralType literal_type, uint8_t tagging_bits = uint8_t(0));

    [[nodiscard]] NodeStorageID manager_id() const;

    [[nodiscard]] uint8_t free_tagging_bits() const;
    void free_tagging_bits(uint8_t new_value);

    [[nodiscard]] const NodeIDValue &node_id() const;

    [[nodiscard]] uint64_t raw() const;

    [[nodiscard]] RDFNodeType type() const;

    [[nodiscard]] bool is_type(RDFNodeType type_) const;

    [[nodiscard]] bool empty() const;

    // TODO: support basic xsd literal datatypes for comparison
    std::partial_ordering operator<=>(const NodeID &other) const;

    bool operator==(const NodeID &other) const;

    // TODO: operator std::string and operator<< overload
    [[nodiscard]] std::string as_string() const;
};


}  // namespace rdf4cpp::rdf::storage::node

#endif  //RDF4CPP_NODEID_HPP
