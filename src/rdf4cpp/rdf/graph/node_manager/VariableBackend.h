#ifndef RDF4CPP_VARIABLEBACKEND_H
#define RDF4CPP_VARIABLEBACKEND_H

#include <rdf4cpp/rdf/graph/node_manager/NodeID.h>

#include <compare>
#include <string>

namespace rdf4cpp::rdf::graph::node_manager {

class VariableBackend {
    std::string name_;
    bool anonymous_;

public:
    explicit VariableBackend(std::string name, bool anonymous = false);
    auto operator<=>(const VariableBackend &) const = default;
    std::strong_ordering operator<=>(VariableBackend const *other) const;

    [[nodiscard]] std::string as_string([[maybe_unused]] bool quoting) const;

    [[nodiscard]] bool is_anonymous() const;

    [[nodiscard]] const std::string &name() const;
};
}  // namespace rdf4cpp::rdf::graph::node_manager

#endif  //RDF4CPP_VARIABLEBACKEND_H
