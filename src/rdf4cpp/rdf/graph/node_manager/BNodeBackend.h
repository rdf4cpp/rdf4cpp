#ifndef RDF4CPP_BNODEBACKEND_H
#define RDF4CPP_BNODEBACKEND_H

#include "ID.h"
#include <string>
namespace rdf4cpp::rdf::graph::node_manager {

class BNodeBackend {
    std::string identifier;

public:
    explicit BNodeBackend(std::string identifier);
    auto operator<=>(const BNodeBackend &) const = default;
    std::weak_ordering operator<=>(BNodeBackend const *other) const;
    [[nodiscard]] std::string as_string([[maybe_unused]] bool quoting) const;
};
}  // namespace rdf4cpp::rdf::graph::node_manager

#endif  //RDF4CPP_BNODEBACKEND_H
