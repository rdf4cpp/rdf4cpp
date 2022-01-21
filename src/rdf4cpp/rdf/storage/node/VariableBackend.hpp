#ifndef RDF4CPP_VARIABLEBACKEND_HPP
#define RDF4CPP_VARIABLEBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/NodeID.hpp>
#include <rdf4cpp/rdf/storage/node/BackendNodeHandles.hpp>

#include <compare>
#include <memory>
#include <string>
#include <string_view>

namespace rdf4cpp::rdf::storage::node {



class VariableBackend {
    std::string name_;
    bool anonymous_;

public:
    explicit VariableBackend(std::string_view name, bool anonymous = false) noexcept;
    auto operator<=>(const VariableBackend &) const noexcept = default;
    std::strong_ordering operator<=>(std::unique_ptr<VariableBackend> const &other) const noexcept;

    [[nodiscard]] bool is_anonymous() const noexcept;

    [[nodiscard]] std::string_view name() const noexcept;

    explicit operator VariableBackendHandle() const noexcept {
        return {.name = name(),
                .is_anonymous = is_anonymous()};
    }
};

std::strong_ordering operator<=>(std::unique_ptr<VariableBackend> const &self, std::unique_ptr<VariableBackend> const &other) noexcept;
}  // namespace rdf4cpp::rdf::storage::node

#endif  //RDF4CPP_VARIABLEBACKEND_HPP
