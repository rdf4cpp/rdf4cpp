#ifndef RDF4CPP_RDFNODE_H
#define RDF4CPP_RDFNODE_H


#include "rdf4cpp/rdf/graph/node_manager/TaggedResourcePointer.h"
#include <optional>
#include <string>

namespace rdf4cpp::rdf::node {
class Literal;
class BlankNode;
class IRIResource;
class Variable;

/**
 * The abstract RDFNode class, containing either a Literal, Variable, IRIResource or BlankNode
 */
class RDFNode {
protected:
    using TaggedResourcePtr = rdf4cpp::rdf::graph::node_manager::TaggedResourcePtr;
    using ID = rdf4cpp::rdf::graph::node_manager::ID;
    using ResourceManager = rdf4cpp::rdf::graph::node_manager::ResourceManager;
    using LiteralBackend = rdf4cpp::rdf::graph::node_manager::LiteralBackend;
    using RDFNodeType = rdf4cpp::rdf::graph::node_manager::RDFNodeType;
    TaggedResourcePtr id_;

    RDFNode(void *ptr, ID id) : id_(ptr, id) {}
    explicit RDFNode(const TaggedResourcePtr &id) : id_(id) {}

public:
    RDFNode() = default;

    // TODO: implement all other "make"-methods
    static Literal make_literal(const std::string &lexical_form,
                                ResourceManager &node_manager = ResourceManager::default_instance());


    [[nodiscard]] std::string as_string(bool quoting) const;
    [[nodiscard]] bool is_literal() const;
    [[nodiscard]] bool is_variable() const;
    [[nodiscard]] bool is_bnode() const;
    [[nodiscard]] bool is_iri() const;
    [[nodiscard]] RDFNodeType type() const;

    bool operator==(const RDFNode &other) const {
        if (this->id_ == other.id_) {
            return true;
        } else if (this->type() != other.type()) {
            return false;
        } else {
            switch (this->type()) {
                case RDFNodeType::IRI:
                    return this->id_.iri() == other.id_.iri();
                case RDFNodeType::BNode:
                    return this->id_.bnode() == other.id_.bnode();
                case RDFNodeType::Literal:
                    return this->id_.literal() == other.id_.literal();
                case RDFNodeType::Variable:
                    return this->id_.variable() == other.id_.variable();
            }
            return false;
        }
    }

    std::weak_ordering operator<=>(const RDFNode &other) const {
        if (auto comp_id = this->id_ <=> other.id_; comp_id == std::strong_ordering::equal) {
            return comp_id;
        } else if (auto comp_type = this->type() <=> other.type(); comp_id != std::strong_ordering::equal) {
            return comp_type;
        } else {  // same type, different id.
            switch (this->type()) {
                // TODO. implement equivalence comparison in the backend types
                case RDFNodeType::IRI:
                    return this->id_.iri() <=> other.id_.iri();
                case RDFNodeType::BNode:
                    return this->id_.bnode() <=> other.id_.bnode();
                case RDFNodeType::Literal:
                    return this->id_.literal() <=> other.id_.literal();
                case RDFNodeType::Variable:
                    return this->id_.variable() <=> other.id_.variable();
            }
            return std::weak_ordering::less;
        }
    }
};
}  // namespace rdf4cpp::rdf::node


#endif  //RDF4CPP_RDFNODE_H
