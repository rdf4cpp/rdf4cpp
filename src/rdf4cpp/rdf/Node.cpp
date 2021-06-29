#include "Node.hpp"

#include <cassert>
#include <rdf4cpp/rdf/BlankNode.hpp>
#include <rdf4cpp/rdf/IRI.hpp>
#include <rdf4cpp/rdf/Literal.hpp>
#include <rdf4cpp/rdf/query/Variable.hpp>
#include <rdf4cpp/rdf/storage/node/IRIBackend.hpp>

namespace rdf4cpp::rdf {

Node::Node(Node::NodeID id) : handle_(id) {}

Node::Node(const Node::BackendNodeHandle &id) : handle_(id) {}

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
bool Node::is_bnode() const {
    return handle_.is_bnode();
}
bool Node::is_iri() const {
    return handle_.is_iri();
}

std::strong_ordering Node::operator<=>(const Node &other) const {
    if (auto comp_id = this->handle_ <=> other.handle_; comp_id == std::partial_ordering::equivalent) {
        return std::strong_ordering::equal;
    } else if (auto comp_type = this->handle_.type() <=> other.handle_.type(); comp_type != std::strong_ordering::equal) {
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
bool Node::operator==(const Node &other) const {
    if (this->handle_ == other.handle_) {
        return true;
    } else if (this->handle_.type() != other.handle_.type()) {
        return false;
    } else {
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
Node::operator BlankNode() const {
    assert(is_bnode());
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
const Node::BackendNodeHandle &Node::backend_handle() const noexcept {
    return handle_;
}
Node::BackendNodeHandle &Node::backend_handle() noexcept {
    return handle_;
}


}  // namespace rdf4cpp::rdf