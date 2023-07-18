#include "NodeBackendHandle.hpp"

#include <rdf4cpp/rdf/storage/node/NodeStorage.hpp>

#include <bit>

namespace rdf4cpp::rdf::storage::node::identifier {

struct __attribute__((__packed__)) NodeBackendHandleImpl {
    NodeID node_id_;
    RDFNodeType type_ : 2;
    NodeStorageID::underlying_type storage_id_ : NodeStorageID::width;
    uint8_t inlined : 1;
    uint8_t free_tagging_bits : 3;
};

static_assert(sizeof(NodeBackendHandleImpl) == 8);
static_assert(sizeof(NodeBackendHandle) == 8);

RDFNodeType NodeBackendHandle::type() const noexcept {
    return std::bit_cast<NodeBackendHandleImpl>(*this).type_;
}
bool NodeBackendHandle::is_iri() const noexcept {
    return type() == RDFNodeType::IRI;
}
bool NodeBackendHandle::is_literal() const noexcept {
    return type() == RDFNodeType::Literal;
}
bool NodeBackendHandle::is_blank_node() const noexcept {
    return type() == RDFNodeType::BNode;
}
bool NodeBackendHandle::is_variable() const noexcept {
    return type() == RDFNodeType::Variable;
}
bool NodeBackendHandle::null() const noexcept {
    return std::bit_cast<NodeBackendHandleImpl>(*this).node_id_.null();
}

view::IRIBackendView NodeBackendHandle::iri_backend() const noexcept {
    return NodeStorage::find_iri_backend_view(*this);
}
view::LiteralBackendView NodeBackendHandle::literal_backend() const noexcept {
    return NodeStorage::find_literal_backend_view(*this);
}
view::BNodeBackendView NodeBackendHandle::bnode_backend() const noexcept {
    return NodeStorage::find_bnode_backend_view(*this);
}
view::VariableBackendView NodeBackendHandle::variable_backend() const noexcept {
    return NodeStorage::find_variable_backend_view(*this);
}

NodeID NodeBackendHandle::node_id() const noexcept {
    return std::bit_cast<NodeBackendHandleImpl>(*this).node_id_;
}

bool NodeBackendHandle::is_inlined() const noexcept {
    return std::bit_cast<NodeBackendHandleImpl>(*this).inlined;
}

uint8_t NodeBackendHandle::free_tagging_bits() const noexcept {
    return std::bit_cast<NodeBackendHandleImpl>(*this).free_tagging_bits;
}
void NodeBackendHandle::set_free_tagging_bits(uint8_t new_value) {
    assert(new_value < (1 << 3));
    auto tmp = std::bit_cast<NodeBackendHandleImpl>(*this);
    tmp.free_tagging_bits = new_value;
    raw_ = std::bit_cast<uint64_t>(tmp);
}

NodeStorageID NodeBackendHandle::node_storage_id() const noexcept {
    return NodeStorageID{std::bit_cast<NodeBackendHandleImpl>(*this).storage_id_};
}

NodeBackendHandle::NodeBackendHandle(uint64_t raw) noexcept
    : raw_{raw} {
}
NodeBackendHandle::NodeBackendHandle(NodeID node_id, RDFNodeType node_type, NodeStorageID node_storage_id, bool inlined, uint8_t tagging_bits) noexcept
    : raw_{std::bit_cast<uint64_t>(NodeBackendHandleImpl{node_id, node_type, node_storage_id.value, inlined, tagging_bits})} {
}

NodeBackendHandle NodeBackendHandle::from_raw(uint64_t raw) noexcept {
    return NodeBackendHandle{raw};
}

uint64_t NodeBackendHandle::raw() const noexcept {
    return raw_;
}

NodeBackendHandle datatype_iri_handle_for_fixed_lit_handle(NodeBackendHandle lit_handle) noexcept {
    assert(lit_handle.is_literal());
    assert(lit_handle.node_id().literal_type().is_fixed());
    return NodeBackendHandle{literal_type_to_iri_node_id(lit_handle.node_id().literal_type()),
                             storage::node::identifier::RDFNodeType::IRI,
                             lit_handle.node_storage_id()};
}

}  // namespace rdf4cpp::rdf::storage::node::identifier