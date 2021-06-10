#include "RDFNode.h"

#include <rdf4cpp/rdf/graph/node_manager/IRIBackend.h>
#include <rdf4cpp/rdf/node/BlankNode.h>
#include <rdf4cpp/rdf/node/IRIResource.h>
#include <rdf4cpp/rdf/node/Literal.h>
#include <rdf4cpp/rdf/node/Variable.h>

namespace rdf4cpp::rdf::node {

RDFNode::RDFNode(void *ptr, RDFNode::NodeID id) : handle_(ptr, id) {}

RDFNode::RDFNode(const RDFNode::BackendNodeHandle &id) : handle_(id) {}

std::string RDFNode::as_string(bool quoting, RDFNode::NodeManager &node_manager) const {
    switch (handle_.type()) {

        case RDFNodeType::IRI:
            return handle_.iri_backend().as_string(quoting);
        case RDFNodeType::BNode:
            return handle_.bnode_backend().as_string(quoting);
        case RDFNodeType::Literal: {
            const auto &literal = static_cast<const Literal &>(*this);
            return literal.as_string(quoting, node_manager);
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
Literal RDFNode::make_string_literal(const std::string &lexical_form, RDFNode::NodeManager &node_manager) {
    const std::pair<rdf4cpp::rdf::graph::node_manager::LiteralBackend *, NodeID> &pair = node_manager.get_string_literal(lexical_form);
    return Literal(pair.first, pair.second);
}

Literal RDFNode::make_typed_literal(const std::string &lexical_form, const IRIResource &datatype, RDFNode::NodeManager &node_manager) {
    NodeID iri_id = node_manager.lookup_id(&datatype.handle_.iri_backend(), RDFNodeType ::IRI);
    const std::pair<rdf4cpp::rdf::graph::node_manager::LiteralBackend *, NodeID> &pair = node_manager.get_typed_literal(lexical_form, iri_id);
    return Literal(pair.first, pair.second);
}

Literal RDFNode::make_typed_literal(const std::string &lexical_form, const std::string &datatype, NodeManager &node_manager) {
    const std::pair<rdf4cpp::rdf::graph::node_manager::LiteralBackend *, NodeID> &pair = node_manager.get_typed_literal(lexical_form, datatype);
    return Literal(pair.first, pair.second);
}

Literal RDFNode::make_lang_literal(const std::string &lexical_form, const std::string &lang, NodeManager &node_manager) {
    const std::pair<rdf4cpp::rdf::graph::node_manager::LiteralBackend *, NodeID> &pair = node_manager.get_lang_literal(lexical_form, lang);
    return Literal(pair.first, pair.second);
}

IRIResource RDFNode::make_iri(const std::string &iri, NodeManager &node_manager) {
    const std::pair<rdf4cpp::rdf::graph::node_manager::IRIBackend *, NodeID> &pair = node_manager.get_iri(iri);
    return IRIResource(pair.first, pair.second);
}

BlankNode RDFNode::make_bnode(const std::string &identifier, NodeManager &node_manager) {
    const std::pair<rdf4cpp::rdf::graph::node_manager::BNodeBackend *, NodeID> &pair = node_manager.get_bnode(identifier);
    return BlankNode(pair.first, pair.second);
}

Variable RDFNode::make_variable(const std::string &identifier, bool anonymous, NodeManager &node_manager) {
    const std::pair<rdf4cpp::rdf::graph::node_manager::VariableBackend *, NodeID> &pair = node_manager.get_variable(identifier, anonymous);
    return Variable(pair.first, pair.second);
}


std::strong_ordering RDFNode::operator<=>(const RDFNode &other) const {
    if (auto comp_id = this->handle_ <=> other.handle_; comp_id == std::strong_ordering::equal) {
        return comp_id;
    } else if (auto comp_type = this->type() <=> other.type(); comp_id != std::strong_ordering::equal) {
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

}  // namespace rdf4cpp::rdf::node