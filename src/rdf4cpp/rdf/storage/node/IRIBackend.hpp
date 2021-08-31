#ifndef RDF4CPP_IRIBACKEND_HPP
#define RDF4CPP_IRIBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/NodeID.hpp>

#include <compare>
#include <string>

namespace rdf4cpp::rdf::storage::node {
class IRIBackend {
    std::string iri;

public:
    explicit IRIBackend(std::string iri);
    // TODO: handle normalization in comparison
    auto operator<=>(const IRIBackend &) const = default;
    std::strong_ordering operator<=>(IRIBackend const *other) const;

    const std::string &identifier() const;

    [[nodiscard]] std::string n_string() const;
};
}  // namespace rdf4cpp::rdf::storage::node

#endif  //RDF4CPP_IRIBACKEND_HPP
