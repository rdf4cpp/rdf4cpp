#include <rdf4cpp/rdf/bnode_management/NodeGenerator.hpp>
#include <rdf4cpp/rdf/bnode_management/reference_backends/scope/ReferenceBNodeScope.hpp>

namespace rdf4cpp::rdf::util {

NodeGenerator::NodeGenerator(std::unique_ptr<IIdGenerator> &&impl) : impl{std::move(impl)} {
}

NodeGenerator &NodeGenerator::default_instance() {
    static NodeGenerator instance = NodeGenerator::with_backend<RandomIdGenerator>();
    return instance;
}

NodeGenerator NodeGenerator::with_backend(std::unique_ptr<IIdGenerator> backend) {
    return NodeGenerator{std::move(backend)};
}

std::string NodeGenerator::generate_id() {
    std::string buf;
    buf.resize(this->impl->max_generated_id_size());

    auto *end = this->impl->generate_to_buf(buf.data());
    buf.resize(end - buf.data());

    return buf;
}

Node NodeGenerator::generate_node(INodeFactory &facade, NodeGenerator::NodeStorage &node_storage) {
    return Node{facade.make_node(*this->impl, node_storage)};
}

NodeScope NodeGenerator::scope() {
    return NodeScope{this->impl.get(), SharedPtr<ReferenceBNodeScope>::make()};
}

}  //namespace rdf4cpp::rdf::util
