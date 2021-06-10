#include "IRIResource.h"

namespace rdf4cpp::rdf::node {

IRIResource::IRIResource(void *ptr, const RDFNode::NodeID &id) : RDFNode(ptr, id) {}
IRIResource::IRIResource() : RDFNode() {}
std::string IRIResource::as_string(bool quoting) const { return handle_.iri_backend().as_string(quoting); }
bool IRIResource::is_blank_node() const { return false; }
bool IRIResource::is_literal() const { return false; }
bool IRIResource::is_variable() const { return false; }
bool IRIResource::is_bnode() const { return false; }
bool IRIResource::is_iri() const { return true; }
RDFNode::RDFNodeType IRIResource::type() const { return RDFNodeType::IRI; }

}  // namespace rdf4cpp::rdf::node