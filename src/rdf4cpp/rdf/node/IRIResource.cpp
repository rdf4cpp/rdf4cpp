#include "IRIResource.h"

namespace rdf4cpp::rdf::node {

IRIResource::IRIResource(const RDFNode::NodeID &id) : RDFNode(id) {}
IRIResource::IRIResource(RDFNode::BackendNodeHandle handle) : RDFNode(handle) {}
IRIResource::IRIResource() : RDFNode() {}
IRIResource::IRIResource(const std::string &iri, RDFNode::NodeManager &node_storage)
    : RDFNode(BackendNodeHandle{node_storage.get_iri(iri).second}) {}

std::string IRIResource::as_string(bool quoting) const { return handle_.iri_backend().as_string(quoting); }

bool IRIResource::is_blank_node() const { return false; }
bool IRIResource::is_literal() const { return false; }
bool IRIResource::is_variable() const { return false; }
bool IRIResource::is_bnode() const { return false; }
bool IRIResource::is_iri() const { return true; }
RDFNode::RDFNodeType IRIResource::type() const { return RDFNodeType::IRI; }


IRIResource IRIResource::default_graph(NodeManager &node_storage) {
    return IRIResource("", node_storage);
}

}  // namespace rdf4cpp::rdf::node