#ifndef RDF4CPP_ID_H
#define RDF4CPP_ID_H

#include "rdf4cpp/rdf/graph/node_manager/RDFNodeType.h"
#include <bitset>
#include <compare>

namespace rdf4cpp::rdf::graph::node_manager {

class ID {
public:
    constexpr static size_t size_t_bits = sizeof(size_t) * 8;

private:
    size_t id_{};
    static size_t add_type_tag(size_t id, RDFNodeType node_type);

public:
    ID() = default;
    explicit ID(size_t id);

    ID(size_t id, RDFNodeType node_type);

    [[nodiscard]] size_t id() const;

    [[nodiscard]] constexpr RDFNodeType type() const;

    [[nodiscard]] constexpr bool is_type(RDFNodeType type_) const;
    ;

    auto operator<=>(const ID &) const = default;
};
}  // namespace rdf4cpp::rdf::graph::node_manager

#endif  //RDF4CPP_ID_H
