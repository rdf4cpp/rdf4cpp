//
// Created by Lixi Alié Conrads on 5/29/21.
//
#include "RDFNode.h"
#include "Literal.h"

std::string rdf4cpp::rdf::node::RDFNode::as_string(bool quoting) const {
    switch (id_.type()) {
        case RDFNodeType::IRI:
            return id_.iri().as_string(quoting);
        case RDFNodeType::BNode:
            return id_.bnode().as_string(quoting);
        case RDFNodeType::Literal:
            return id_.literal().as_string(quoting);
        case RDFNodeType::Variable:
            return id_.variable().as_string(quoting);
    }
    return "";
}
bool rdf4cpp::rdf::node::RDFNode::is_literal() const {
    return id_.is_literal();
}
bool rdf4cpp::rdf::node::RDFNode::is_variable() const {
    return id_.is_variable();
}
bool rdf4cpp::rdf::node::RDFNode::is_bnode() const {
    return id_.is_bnode();
}
bool rdf4cpp::rdf::node::RDFNode::is_iri() const {
    return id_.is_iri();
}
rdf4cpp::rdf::node::RDFNode::RDFNodeType rdf4cpp::rdf::node::RDFNode::type() const {
    return id_.type();
}
rdf4cpp::rdf::node::Literal rdf4cpp::rdf::node::RDFNode::make_literal(const std::string &lexical_form, rdf4cpp::rdf::node::RDFNode::ResourceManager &node_manager) {
    const std::pair<LiteralBackend *, ID> &pair = node_manager.getLiteral(lexical_form);
    // TODO: make Literal directly
    return Literal(pair.first, pair.second);
}
