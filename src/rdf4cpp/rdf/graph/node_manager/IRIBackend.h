#ifndef RDF4CPP_IRIBACKEND_H
#define RDF4CPP_IRIBACKEND_H
#include "ID.h"
#include <string>
namespace rdf4cpp::rdf::graph::node_manager {
class IRIBackend {
    std::string iri;

public:
    explicit IRIBackend(std::string iri);
    auto operator<=>(const IRIBackend &) const = default;
    std::weak_ordering operator<=>(IRIBackend const *other) const;
    [[nodiscard]] std::string as_string([[maybe_unused]] bool quoting) const;
};
}  // namespace rdf4cpp::rdf::graph::node_manager

#endif  //RDF4CPP_IRIBACKEND_H
