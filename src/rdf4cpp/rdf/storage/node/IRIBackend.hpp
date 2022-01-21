#ifndef RDF4CPP_IRIBACKEND_HPP
#define RDF4CPP_IRIBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/NodeID.hpp>

#include <compare>
#include <memory>
#include <string>
#include <string_view>

namespace rdf4cpp::rdf::storage::node {

struct IRIBackendHandle {
    std::string_view identifier;

    [[nodiscard]] std::string n_string() const noexcept {
        return "<" + std::string{identifier} + ">";
    }

    auto operator<=>( IRIBackendHandle const&) const noexcept = default;
};

class IRIBackend {
    std::string iri;

public:
    explicit IRIBackend(std::string_view iri) noexcept;
    // TODO: handle normalization in comparison
    auto operator<=>(const IRIBackend &) const = default;
    std::strong_ordering operator<=>(std::unique_ptr<IRIBackend> const &other) const noexcept;

    [[nodiscard]] std::string_view identifier() const noexcept;


    explicit operator IRIBackendHandle() const noexcept {
        return {.identifier = identifier()};
    }
};
std::strong_ordering operator<=>(std::unique_ptr<IRIBackend> const &self, std::unique_ptr<IRIBackend> const &other) noexcept;
}  // namespace rdf4cpp::rdf::storage::node

#endif  //RDF4CPP_IRIBACKEND_HPP
