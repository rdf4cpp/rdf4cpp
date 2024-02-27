#include "Variable.hpp"

#include <rdf4cpp/rdf/writer/TryWrite.hpp>

namespace rdf4cpp::rdf::query {
Variable::Variable() noexcept : Node(NodeBackendHandle{{}, storage::node::identifier::RDFNodeType::Variable, {}}) {}
Variable::Variable(std::string_view name, bool anonymous, NodeStorage &node_storage)
    : Node{NodeBackendHandle{node_storage.find_or_make_id(storage::node::view::VariableBackendView{.name = name, .is_anonymous = anonymous}),
                             storage::node::identifier::RDFNodeType::Variable,
                             node_storage.id()}} {}
Variable::Variable(Node::NodeBackendHandle handle) noexcept : Node{handle} {}

Variable Variable::make_named(std::string_view name, Node::NodeStorage &node_storage) {
    return Variable{name, false, node_storage};
}

Variable Variable::make_anonymous(std::string_view name, Node::NodeStorage &node_storage) {
    return Variable{name, true, node_storage};
}

Variable Variable::to_node_storage(NodeStorage &node_storage) const noexcept {
    if (handle_.node_storage_id() == node_storage.id()) {
        return *this;
    }

    auto const node_id = node_storage.find_or_make_id(NodeStorage::find_variable_backend_view(handle_));
    return Variable{NodeBackendHandle{node_id, storage::node::identifier::RDFNodeType::Variable, node_storage.id()}};
}

Variable Variable::try_get_in_node_storage(NodeStorage const &node_storage) const noexcept {
    if (handle_.node_storage_id() == node_storage.id()) {
        return *this;
    }

    auto const node_id = node_storage.find_id(NodeStorage::find_variable_backend_view(handle_));
    if (node_id == NodeID{}) {
        return Variable{};
    }

    return Variable{NodeBackendHandle{node_id, storage::node::identifier::RDFNodeType::Variable, node_storage.id()}};
}

Variable Variable::find(std::string_view name, bool anonymous, NodeStorage& node_storage) noexcept {
    auto nid = node_storage.find_id(storage::node::view::VariableBackendView{.name = name, .is_anonymous = anonymous});

    if (nid.null())
        return Variable{};

    return Variable{NodeBackendHandle{nid, storage::node::identifier::RDFNodeType::Variable, node_storage.id()}};
}
Variable Variable::find_named(std::string_view name, NodeStorage& node_storage) noexcept {
    return find(name, false, node_storage);
}
Variable Variable::find_anonymous(std::string_view name, NodeStorage& node_storage) noexcept {
    return find(name, true, node_storage);
}

bool Variable::is_anonymous() const {
    // TODO: encode is_anonymous into variable ID
    return this->handle_.variable_backend().is_anonymous;
}
std::string_view Variable::name() const {
    return this->handle_.variable_backend().name;
}

bool Variable::serialize(void *const buffer, writer::Cursor *const cursor, writer::FlushFunc const flush) const noexcept {
    auto const backend = handle_.variable_backend();

    if (backend.is_anonymous) {
        RDF4CPP_DETAIL_TRY_WRITE_STR("_:");
    } else {
        RDF4CPP_DETAIL_TRY_WRITE_STR("?");
    }

    RDF4CPP_DETAIL_TRY_WRITE_STR(backend.name);
    return true;
}

Variable::operator std::string() const {
    return writer::StringWriter::oneshot([this](auto &w) noexcept {
        return this->serialize(w);
    });
}

bool Variable::is_literal() const { return false; }
bool Variable::is_variable() const { return true; }
bool Variable::is_blank_node() const { return false; }
bool Variable::is_iri() const { return false; }
Node::RDFNodeType Variable::type() const { return RDFNodeType::Variable; }
std::ostream &operator<<(std::ostream &os, Variable const &variable) {
    writer::BufOStreamWriter w{os};
    variable.serialize(w);
    w.finalize();

    return os;
}

}  // namespace rdf4cpp::rdf::query