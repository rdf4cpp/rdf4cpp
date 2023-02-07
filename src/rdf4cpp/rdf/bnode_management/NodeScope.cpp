#include <rdf4cpp/rdf/bnode_management/NodeScope.hpp>
#include <rdf4cpp/rdf/bnode_management/reference_backends/scope/ReferenceBNodeScope.hpp>

namespace rdf4cpp::rdf::util {

NodeScope::NodeScope(IIdGenerator *generator_backend, SharedPtr<INodeScope> scope_backend) noexcept : generator{generator_backend},
                                                                                                      scope{std::move(scope_backend)} {
}

NodeScope::NodeScope(NodeScope &&other) noexcept : generator{other.generator},
                                                   scope{std::move(other.scope)} {
}

NodeScope NodeScope::subscope(std::string scope_name) noexcept {
    return NodeScope{this->generator, this->scope->subscope(std::move(scope_name))};
}

Node NodeScope::generate_node(NodeStorage &node_storage) {
    return Node{this->scope->make_node(*this->generator, node_storage)};
}

Node NodeScope::try_get_node(std::string_view label) const noexcept {
    return Node{this->scope->find_node(label)};
}

Node NodeScope::get_or_generate_node(std::string_view label, NodeStorage &node_storage) {
    if (Node node = this->try_get_node(label); !node.null()) {
        return node;
    }

    Node node = this->generate_node(node_storage);
    this->scope->label_node(label, node.backend_handle());

    return node;
}

}  //namespace rdf4cpp::rdf::util
