#ifndef RDF4CPP_NODEID_H
#define RDF4CPP_NODEID_H

#include <rdf4cpp/rdf/graph/node_manager/RDFNodeType.h>

#include <bitset>
#include <compare>
#include <string>
#include <utility>
#include <vector>

namespace rdf4cpp::rdf::graph::node_manager {

class NodeID {
public:
    constexpr static size_t size_t_bits = sizeof(size_t) * 8;

private:
    size_t id_{};
    static size_t add_type_tag(size_t id, RDFNodeType node_type);

public:
    NodeID() = default;
    explicit NodeID(size_t id);

    NodeID(size_t id, RDFNodeType node_type);

    [[nodiscard]] size_t id() const;

    [[nodiscard]] constexpr RDFNodeType type() const;

    [[nodiscard]] constexpr bool is_type(RDFNodeType type_) const;

    [[nodiscard]] constexpr bool empty() const;

    auto operator<=>(const NodeID &) const = default;

    static const std::pair<NodeID, std::string> xsd_string_iri;
    static const std::pair<NodeID, std::string> xsd_langstring_iri;

    static const std::vector<std::pair<NodeID, std::string>> predefined_iris;
    static const size_t min_iri_id;
    static const size_t min_literal_id;
    static const size_t min_bnode_id;
    static const size_t min_variable_id;
};


}  // namespace rdf4cpp::rdf::graph::node_manager

#endif  //RDF4CPP_NODEID_H
