#include "Literal.h"
namespace rdf4cpp::rdf::node {

Literal::Literal(void *ptr, const RDFNode::ID &id) : RDFNode(ptr, id) {}
}  // namespace rdf4cpp::rdf::node