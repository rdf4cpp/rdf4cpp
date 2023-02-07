#include <rdf4cpp/rdf/bnode_management/reference_backends/scope/ReferenceBNodeScope.hpp>

namespace rdf4cpp::rdf::util {

storage::node::identifier::NodeBackendHandle ReferenceBNodeScope::make_null_node() const noexcept {
    return storage::node::identifier::NodeBackendHandle{{}, storage::node::identifier::RDFNodeType::BNode, {}};
}

storage::node::identifier::NodeBackendHandle ReferenceBNodeScope::make_node(IIdGenerator &generator, storage::node::NodeStorage &node_storage) {
    using namespace storage::node;

    size_t const buf_sz = generator.max_generated_id_size();
    auto buf = std::make_unique<char[]>(buf_sz);
    auto *end = generator.generate_to_buf(buf.get());

    auto id = node_storage.find_or_make_id(view::BNodeBackendView{.identifier = std::string_view{buf.get(), static_cast<size_t>(end - buf.get())},
                                                                  .scope = this->weak_from_this()});
    identifier::NodeBackendHandle handle{id, identifier::RDFNodeType::BNode, node_storage.id()};
    return handle;
}

}  //namespace rdf4cpp::rdf::util
