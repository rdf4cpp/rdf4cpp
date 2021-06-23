#include "RDFNode.h"

#include <cassert>
#include <rdf4cpp/rdf/graph/node_storage/IRIBackend.h>
#include <rdf4cpp/rdf/node/BlankNode.h>
#include <rdf4cpp/rdf/node/IRIResource.h>
#include <rdf4cpp/rdf/node/Literal.h>
#include <rdf4cpp/rdf/node/Variable.h>

namespace rdf4cpp::rdf::node {

RDFNode::RDFNode(RDFNode::NodeID id) : handle_(id) {}

RDFNode::RDFNode(const RDFNode::BackendNodeHandle &id) : handle_(id) {}

std::string RDFNode::as_string(bool quoting) const {
    switch (handle_.type()) {

        case RDFNodeType::IRI:
            return handle_.iri_backend().as_string(quoting);
        case RDFNodeType::BNode:
            return handle_.bnode_backend().as_string(quoting);
        case RDFNodeType::Literal: {
            const auto &literal = static_cast<const Literal &>(*this);
            return literal.as_string(quoting);
        }
        case RDFNodeType::Variable:
            return handle_.variable_backend().as_string(quoting);
    }
    return "";
}
bool RDFNode::is_literal() const {
    return handle_.is_literal();
}
bool RDFNode::is_variable() const {
    return handle_.is_variable();
}
bool RDFNode::is_bnode() const {
    return handle_.is_bnode();
}
bool RDFNode::is_iri() const {
    return handle_.is_iri();
}
RDFNode::RDFNodeType RDFNode::type() const {
    return handle_.type();
}

std::strong_ordering RDFNode::operator<=>(const RDFNode &other) const {
    if (auto comp_id = this->handle_ <=> other.handle_; comp_id == std::partial_ordering::equivalent) {
        return std::strong_ordering::equal;
    } else if (auto comp_type = this->type() <=> other.type(); comp_type != std::strong_ordering::equal) {
        return comp_type;
    } else {  // same type, different id.
        switch (this->type()) {
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
bool RDFNode::operator==(const RDFNode &other) const {
    if (this->handle_ == other.handle_) {
        return true;
    } else if (this->type() != other.type()) {
        return false;
    } else {
        switch (this->type()) {
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
RDFNode::operator BlankNode() const {
    assert(is_bnode());
    return BlankNode{handle_};
}
RDFNode::operator IRIResource() const {
    assert(is_iri());
    return IRIResource(handle_);
}
RDFNode::operator Literal() const {
    assert(is_literal());
    return Literal(handle_);
}
RDFNode::operator Variable() const {
    assert(is_variable());
    return Variable(handle_);
}

}  // namespace rdf4cpp::rdf::node