#ifndef RDF4CPP_BNODEBACKEND_HPP
#define RDF4CPP_BNODEBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/NodeID.hpp>

#include <compare>
#include <memory>
#include <string>
#include <string_view>

namespace rdf4cpp::rdf::storage::node {

struct BNodeBackendHandle {
    std::string_view identifier;
    [[nodiscard]] std::string n_string() const noexcept {
        return "_:" + std::string{identifier};
    }
    auto operator<=>( BNodeBackendHandle const&) const noexcept = default;
};

class BNodeBackend {
    std::string identifier_;

public:
    explicit BNodeBackend(std::string_view identifier) noexcept;
    auto operator<=>(const BNodeBackend &) const noexcept = default;
    std::strong_ordering operator<=>(std::unique_ptr<BNodeBackend> const &other) const noexcept;

    [[nodiscard]] std::string_view identifier() const noexcept;

    explicit operator BNodeBackendHandle() const noexcept {
        return {.identifier = identifier()};
    }
};

std::strong_ordering operator<=>(std::unique_ptr<BNodeBackend> const &self, std::unique_ptr<BNodeBackend> const &other) noexcept;
}  // namespace rdf4cpp::rdf::storage::node


#endif  //RDF4CPP_BNODEBACKEND_HPP
