#include <rdf4cpp/rdf/bnode_management/reference_backends/factory/UnscopedBNodeFactory.hpp>

namespace rdf4cpp::rdf::util {

UnscopedBNodeFactory &UnscopedBNodeFactory::default_instance() noexcept {
    static UnscopedBNodeFactory instance;
    return instance;
}

storage::node::identifier::NodeBackendHandle UnscopedBNodeFactory::make_null_node() const noexcept {
    return storage::node::identifier::NodeBackendHandle{{}, storage::node::identifier::RDFNodeType::BNode, {}};
}

storage::node::identifier::NodeBackendHandle UnscopedBNodeFactory::make_node(IIdGenerator &generator, storage::node::NodeStorage &node_storage) {
    size_t const buf_sz = generator.max_generated_id_size();
    auto buf = std::make_unique<char[]>(buf_sz);
    auto *end = generator.generate_to_buf(buf.get());

    auto const node_id = node_storage.find_or_make_id(storage::node::view::BNodeBackendView{.identifier = std::string_view{buf.get(), static_cast<size_t>(end - buf.get())},
                                                                                            .scope = nullptr});

    return storage::node::identifier::NodeBackendHandle{node_id, storage::node::identifier::RDFNodeType::BNode, node_storage.id()};
}

}  //namespace rdf4cpp::rdf::util
