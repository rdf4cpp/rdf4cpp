#include <rdf4cpp/rdf/bnode_mngt/reference_backends/factory/BNodeFactory.hpp>

#include <rdf4cpp/rdf/bnode_mngt/NodeScope.hpp>

namespace rdf4cpp::rdf::bnode_mngt {

BNodeFactory &BNodeFactory::default_instance() noexcept {
    static BNodeFactory instance;
    return instance;
}

storage::node::identifier::NodeBackendHandle BNodeFactory::make_node(IIdGenerator &generator,
                                                                     NodeScope const *scope,
                                                                     storage::node::DynNodeStorage node_storage) noexcept {
    size_t const buf_sz = generator.max_generated_id_size();
    auto buf = std::make_unique<char[]>(buf_sz);
    auto *end = generator.generate_to_buf(buf.get());

    std::string_view const identifier{buf.get(), static_cast<size_t>(end - buf.get())};

    auto const id = [&]() noexcept {
        if (scope != nullptr) {
            WeakNodeScope weak = scope->downgrade();
            return node_storage.find_or_make_id(storage::node::view::BNodeBackendView{.identifier = identifier,
                                                                                      .scope = weak});
        } else {
            return node_storage.find_or_make_id(storage::node::view::BNodeBackendView{.identifier = identifier,
                                                                                      .scope = std::nullopt});
        }
    }();

    return storage::node::identifier::NodeBackendHandle{id, storage::node::identifier::RDFNodeType::BNode, node_storage};
}

}  //namespace rdf4cpp::rdf::bnode_mngt
