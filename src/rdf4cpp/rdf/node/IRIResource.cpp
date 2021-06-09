#include "IRIResource.h"
namespace rdf4cpp::rdf::node {

IRIResource::IRIResource(void *ptr, const RDFNode::ID &id) : RDFNode(ptr, id) {}
}  // namespace rdf4cpp::rdf::node