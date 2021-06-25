#ifndef RDF4CPP_BNODEBACKEND_HPP
#define RDF4CPP_BNODEBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/NodeID.hpp>

#include <compare>
#include <string>

namespace rdf4cpp::rdf::storage::node {

class BNodeBackend {
    std::string identifier_;

public:
    explicit BNodeBackend(std::string identifier);
    auto operator<=>(const BNodeBackend &) const = default;
    std::strong_ordering operator<=>(BNodeBackend const *other) const;
    [[nodiscard]] std::string as_string([[maybe_unused]] bool quoting) const;
    [[nodiscard]] const std::string &indentifier() const;
};
}  // namespace rdf4cpp::rdf::storage::node

#endif  //RDF4CPP_BNODEBACKEND_HPP
