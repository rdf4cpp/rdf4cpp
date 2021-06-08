#ifndef RDF4CPP_RDFNODE_H
#define RDF4CPP_RDFNODE_H


#include <optional>
#include <rdf4cpp/rdf/graph/node_manager/TaggedResourcePointer.h>
#include <string>

namespace rdf4cpp::rdf::node {
class Literal;

///
/// The abstract RDFNode class, containing either a Literal, Variable, IRIResource or BlankNode
///
class RDFNode {
protected:
    using TaggedResourcePtr = rdf4cpp::rdf::graph::node_manager::TaggedResourcePtr;
    using ID = rdf4cpp::rdf::graph::node_manager::ID;
    using ResourceManager = rdf4cpp::rdf::graph::node_manager::ResourceManager;
    using LiteralBackend = rdf4cpp::rdf::graph::node_manager::LiteralBackend;
    using RDFNodeType = rdf4cpp::rdf::graph::node_manager::RDFNodeType;
    TaggedResourcePtr id_;

    RDFNode(void *ptr, ID id) : id_(ptr, id) {}

public:
    RDFNode() = default;
    explicit RDFNode(const TaggedResourcePtr &id) : id_(id) {}

    static Literal make_literal(const std::string &lexical_form,
                                ResourceManager &node_manager = ResourceManager::default_instance());


    [[nodiscard]] std::string as_string(bool quoting) const;

    [[nodiscard]] bool is_literal() const;

    [[nodiscard]] bool is_variable() const;

    [[nodiscard]] bool is_bnode() const;

    [[nodiscard]] bool is_iri() const;

    [[nodiscard]] RDFNodeType type() const;
};
}  // namespace rdf4cpp::rdf::node


#endif  //RDF4CPP_RDFNODE_H
