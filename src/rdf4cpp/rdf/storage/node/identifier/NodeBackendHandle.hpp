#ifndef RDF4CPP_NODEBACKENDHANDLE_HPP
#define RDF4CPP_NODEBACKENDHANDLE_HPP

#include <rdf4cpp/rdf/storage/node/handle/BNodeBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/handle/IRIBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/handle/LiteralBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/handle/VariableBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/NodeStorageID.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/RDFNodeType.hpp>


#include <compare>
#include <cstddef>

namespace rdf4cpp::rdf::storage::node::identifier {

class NodeBackendHandle {
private:
    uint64_t raw_{};

public:
    NodeBackendHandle() noexcept = default;

    explicit NodeBackendHandle(NodeID node_id, RDFNodeType node_tpye, NodeStorageID node_storage_id, uint8_t tagging_bits = {}) noexcept;

    [[nodiscard]] RDFNodeType type() const noexcept;

    [[nodiscard]] bool is_iri() const noexcept;
    [[nodiscard]] bool is_literal() const noexcept;
    [[nodiscard]] bool is_blank_node() const noexcept;
    [[nodiscard]] bool is_variable() const noexcept;

    [[nodiscard]] bool empty() const noexcept;

    [[nodiscard]] NodeStorageID node_storage_id() const noexcept;

    [[nodiscard]] NodeID node_id() const noexcept;

    [[nodiscard]] uint8_t free_tagging_bits() const noexcept;
    void set_free_tagging_bits(uint8_t new_value);

    [[nodiscard]] uint64_t raw() const noexcept;

    std::partial_ordering operator<=>(NodeBackendHandle const &other) const noexcept = default;

    bool operator==(NodeBackendHandle const &other) const noexcept = default;

    [[nodiscard]] handle::IRIBackendView iri_backend() const noexcept;
    [[nodiscard]] handle::LiteralBackendView literal_backend() const noexcept;
    [[nodiscard]] handle::BNodeBackendView bnode_backend() const noexcept;
    [[nodiscard]] handle::VariableBackendView variable_backend() const noexcept;
};
static_assert(sizeof(NodeBackendHandle) == sizeof(uint64_t));
}  // namespace rdf4cpp::rdf::storage::node::identifier

#include <rdf4cpp/rdf/storage/node/NodeStorage.hpp>

template<>
struct std::hash<rdf4cpp::rdf::storage::node::identifier::NodeBackendHandle> {
    inline size_t operator()(rdf4cpp::rdf::storage::node::identifier::NodeBackendHandle const &v) const noexcept {
        return v.raw();
    }
};

#endif  //RDF4CPP_NODEBACKENDHANDLE_HPP
