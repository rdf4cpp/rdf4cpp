#ifndef RDF4CPP_IRIBACKEND_HPP
#define RDF4CPP_IRIBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/handle/IRIBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>

#include <compare>
#include <memory>
#include <string>
#include <string_view>

namespace rdf4cpp::rdf::storage::node::default_node_storage {


class IRIBackend {
    std::string iri;

public:
    explicit IRIBackend(std::string_view iri) noexcept;
    auto operator<=>(const IRIBackend &) const = default;
    std::strong_ordering operator<=>(std::unique_ptr<IRIBackend> const &other) const noexcept;

    [[nodiscard]] std::string_view identifier() const noexcept;

    explicit operator handle::IRIBackendView() const noexcept;
};
std::strong_ordering operator<=>(std::unique_ptr<IRIBackend> const &self, std::unique_ptr<IRIBackend> const &other) noexcept;
}  // namespace rdf4cpp::rdf::storage::node::default_node_storage

#endif  //RDF4CPP_IRIBACKEND_HPP
