#ifndef RDF4CPP_VARIABLEBACKEND_H
#define RDF4CPP_VARIABLEBACKEND_H
#include "ID.h"
#include <string>
namespace rdf4cpp::rdf::graph::node_manager {

class VariableBackend {
    std::string name;
    bool anonymous;

public:
    explicit VariableBackend(std::string name, bool anonymous = false);
    auto operator<=>(const VariableBackend &) const = default;
    std::weak_ordering operator<=>(VariableBackend const *other) const;

    [[nodiscard]] std::string as_string([[maybe_unused]] bool quoting) const;
};
}  // namespace rdf4cpp::rdf::graph::node_manager

#endif  //RDF4CPP_VARIABLEBACKEND_H
