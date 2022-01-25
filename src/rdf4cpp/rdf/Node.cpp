#include "Node.hpp"

#include <cassert>
#include <rdf4cpp/rdf/BlankNode.hpp>
#include <rdf4cpp/rdf/IRI.hpp>
#include <rdf4cpp/rdf/Literal.hpp>
#include <rdf4cpp/rdf/query/Variable.hpp>
#include <rdf4cpp/rdf/storage/node/IRIBackend.hpp>

namespace rdf4cpp::rdf {

Node::Node(Node::NodeID id) : handle_(id) {}

Node::Node(const Node::NodeBackendHandle &id) : handle_(id) {}

Node Node::to_node_storage(Node::NodeStorage &node_storage) const {
    if (this->backend_handle().node_storage() == node_storage)
        return *this;
    else {
        NodeID node_id = [&]() {
            switch (this->backend_handle().type()) {

                case RDFNodeType::Variable: {
                    auto variable = static_cast<query::Variable>(*this);
                    return node_storage.get_variable_id(variable.name(), variable.is_anonymous());
                }
                case RDFNodeType::BNode: {
                    auto bnode = static_cast<BlankNode>(*this);
                    return node_storage.get_bnode_id(bnode.identifier());
                }
                case RDFNodeType::IRI: {
                    auto iri = static_cast<IRI>(*this);
                    return node_storage.get_variable_id(iri.identifier());
                }
                case RDFNodeType::Literal: {
                    auto literal = static_cast<Literal>(*this);
                    if (literal.backend_handle().literal_backend().datatype_id.node_id() == storage::node::identifier::NodeID::rdf_langstring_iri.first)
                        return node_storage.get_lang_literal_id(literal.lexical_form(), literal.language_tag());
                    else
                        return node_storage.get_typed_literal_id(literal.lexical_form(), literal.datatype().identifier());
                }
                default:
                    return NodeID{};
            }
        }();
        return Node(node_id);
    }
}

Node::operator std::string() const {
    switch (handle_.type()) {

        case RDFNodeType::IRI:
            return handle_.iri_backend().n_string();
        case RDFNodeType::BNode:
            return handle_.bnode_backend().n_string();
        case RDFNodeType::Literal: {
            const auto &literal = static_cast<const Literal &>(*this);
            return (std::string) literal;
        }
        case RDFNodeType::Variable:
            return handle_.variable_backend().n_string();
    }
    return "";
}

bool Node::is_literal() const {
    return handle_.is_literal();
}
bool Node::is_variable() const {
    return handle_.is_variable();
}
bool Node::is_blank_node() const {
    return handle_.is_blank_node();
}
bool Node::is_iri() const {
    return handle_.is_iri();
}

std::partial_ordering Node::operator<=>(const Node &other) const {
    [[likely]] if (this->handle_.id().manager_id() == other.handle_.id().manager_id()) {  // same NodeStorage
        return std::make_tuple(this->handle_.id().type(), this->handle_.id().node_id()) <=> std::make_tuple(other.handle_.id().type(), other.handle_.id().node_id());
    }
    else {  // different NodeStorage
        if (auto comp_type = this->handle_.type() <=> other.handle_.type(); comp_type != std::strong_ordering::equal) {
            return comp_type;
        } else {  // same type, different id.
            switch (this->handle_.type()) {
                case RDFNodeType::IRI:
                    return this->handle_.iri_backend() <=> other.handle_.iri_backend();
                case RDFNodeType::BNode:
                    return this->handle_.bnode_backend() <=> other.handle_.bnode_backend();
                case RDFNodeType::Literal:
                    return this->handle_.literal_backend() <=> other.handle_.literal_backend();
                case RDFNodeType::Variable:
                    return this->handle_.variable_backend() <=> other.handle_.variable_backend();
            }
            return std::strong_ordering::less;
        }
    }
}

bool Node::operator==(const Node &other) const {
    [[likely]] if (this->handle_.id().manager_id() == other.handle_.id().manager_id()) {  // same NodeStorage
        return std::make_tuple(this->handle_.id().type(), this->handle_.id().node_id()) == std::make_tuple(other.handle_.id().type(), other.handle_.id().node_id());
    }
    else {  // different NodeStorage
        if (this->handle_.type() != other.handle_.type()) {
            return false;
        } else {  // same type, different id.
            switch (this->handle_.type()) {
                case RDFNodeType::IRI:
                    return this->handle_.iri_backend() == other.handle_.iri_backend();
                case RDFNodeType::BNode:
                    return this->handle_.bnode_backend() == other.handle_.bnode_backend();
                case RDFNodeType::Literal:
                    return this->handle_.literal_backend() == other.handle_.literal_backend();
                case RDFNodeType::Variable:
                    return this->handle_.variable_backend() == other.handle_.variable_backend();
            }
            return false;
        }
    }
}
bool operator==(const Node &lhs, const std::unique_ptr<Node> &rhs) noexcept {
    return lhs == *rhs;
}

bool operator==(const std::unique_ptr<Node> &lhs, Node const &rhs) noexcept {
    return *lhs == rhs;
}

bool operator==(const std::unique_ptr<Node> &lhs, const std::unique_ptr<Node> &rhs) noexcept {
    return *lhs == *rhs;
}

bool operator==(const Node *lhs, const std::unique_ptr<Node> &rhs) noexcept {
    return *lhs == *rhs;
}

bool operator==(const std::unique_ptr<Node> &lhs, const Node *rhs) noexcept {
    return *lhs == *rhs;
}

Node::operator BlankNode() const {
    assert(is_blank_node());
    return BlankNode{handle_};
}
Node::operator IRI() const {
    assert(is_iri());
    return IRI(handle_);
}
Node::operator Literal() const {
    assert(is_literal());
    return Literal(handle_);
}
Node::operator query::Variable() const {
    assert(is_variable());
    return query::Variable(handle_);
}
bool Node::null() const noexcept {
    return handle_.empty();
}
std::ostream &operator<<(std::ostream &os, const Node &node) {
    return os << (std::string) node;
}
const Node::NodeBackendHandle &Node::backend_handle() const noexcept {
    return handle_;
}
Node::NodeBackendHandle &Node::backend_handle() noexcept {
    return handle_;
}


}  // namespace rdf4cpp::rdf