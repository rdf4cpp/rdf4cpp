#include <rdf4cpp/rdf/bnode_mngt/NodeGenerator.hpp>

namespace rdf4cpp::rdf::bnode_mngt {

NodeGenerator::NodeGenerator(std::unique_ptr<IIdGenerator> &&generator, std::unique_ptr<INodeFactory> &&factory) noexcept : generator{std::move(generator)},
                                                                                                                            factory{std::move(factory)} {
}

NodeGenerator &NodeGenerator::default_instance() {
    static NodeGenerator instance = NodeGenerator::new_instance();
    return instance;
}

NodeGenerator NodeGenerator::new_instance() noexcept {
    return NodeGenerator::new_instance<RandomIdGenerator, BNodeFactory>();
}

NodeGenerator NodeGenerator::with_backends(std::unique_ptr<IIdGenerator> generator, std::unique_ptr<INodeFactory> factory) {
    return NodeGenerator{std::move(generator), std::move(factory)};
}

std::string NodeGenerator::generate_id() {
    std::string buf;
    buf.resize(this->generator->max_generated_id_size());

    auto *end = this->generator->generate_to_buf(buf.data());
    buf.resize(end - buf.data());

    return buf;
}

Node NodeGenerator::generate_node_impl(NodeScope const *scope, storage::node::DynNodeStorage node_storage) {
    return Node{factory->make_node(*this->generator, scope, node_storage)};
}

Node NodeGenerator::generate_node(storage::node::DynNodeStorage node_storage) {
    return generate_node_impl(nullptr, node_storage);
}

}  //namespace rdf4cpp::rdf::bnode_mngt
