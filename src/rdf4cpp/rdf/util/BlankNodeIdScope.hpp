#ifndef RDF4CPP_RDF_UTIL_BLANKNODEIDSCOPE_HPP
#define RDF4CPP_RDF_UTIL_BLANKNODEIDSCOPE_HPP

#include <rdf4cpp/rdf/util/IBlankNodeScopeBackend.hpp>

#include <memory>
#include <shared_mutex>
#include <string>
#include <string_view>

namespace rdf4cpp::rdf::util {

struct BlankNodeIdGenerator;

struct BlankNodeIdScope {
    using generator_type = BlankNodeIdGenerator;
    using NodeStorage = storage::node::NodeStorage;

private:
    friend struct BlankNodeIdGenerator;
    IBlankNodeScopeBackend *backend;
    bool owned;

    explicit BlankNodeIdScope(IBlankNodeScopeBackend *backend, bool owned) noexcept;

public:
    explicit BlankNodeIdScope(generator_type &generator);
    BlankNodeIdScope(BlankNodeIdScope &&other) noexcept;
    ~BlankNodeIdScope() noexcept;

    [[nodiscard]] BlankNode generate_bnode(NodeStorage &node_storage = NodeStorage::default_instance());
    [[nodiscard]] IRI generate_skolem_iri(std::string_view iri_prefix, NodeStorage &node_storage = NodeStorage::default_instance());

    [[nodiscard]] BlankNode try_get_bnode(std::string_view label) const noexcept;
    [[nodiscard]] IRI try_get_skolem_iri(std::string_view label) const noexcept;

    [[nodiscard]] BlankNode get_or_generate_bnode(std::string_view label, NodeStorage &node_storage = NodeStorage::default_instance());
    [[nodiscard]] IRI get_or_generate_skolem_iri(std::string_view iri_prefix, std::string_view label, NodeStorage &node_storage = NodeStorage::default_instance());

    [[nodiscard]] BlankNodeIdScope subscope(std::string const &scope_name) noexcept;
    [[nodiscard]] BlankNodeIdScope subscope(std::string &&scope_name) noexcept;
};

}  //namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_RDF_UTIL_BLANKNODEIDSCOPE_HPP
