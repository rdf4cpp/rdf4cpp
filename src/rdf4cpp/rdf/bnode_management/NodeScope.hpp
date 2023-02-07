#ifndef RDF4CPP_RDF_UTIL_BLANKNODEIDSCOPE_HPP
#define RDF4CPP_RDF_UTIL_BLANKNODEIDSCOPE_HPP

#include <rdf4cpp/rdf/Node.hpp>
#include <rdf4cpp/rdf/bnode_management/IIdGenerator.hpp>
#include <rdf4cpp/rdf/bnode_management/INodeScope.hpp>
#include <rdf4cpp/rdf/util/SharedPtr.hpp>

#include <rdf4cpp/rdf/bnode_management/reference_backends/scope/ReferenceBNodeScope.hpp>
#include <rdf4cpp/rdf/bnode_management/reference_backends/scope/ReferenceSkolemIRIScope.hpp>

#include <string>
#include <string_view>

namespace rdf4cpp::rdf::util {

struct NodeGenerator;

struct NodeScope {
    using NodeStorage = storage::node::NodeStorage;

private:
    friend NodeGenerator;

    IIdGenerator *generator;
    SharedPtr<INodeScope> scope;

    explicit NodeScope(IIdGenerator *generator_backend, SharedPtr<INodeScope> scope_backend) noexcept;
public:
    NodeScope(NodeScope &&other) noexcept;

    [[nodiscard]] Node generate_node(NodeStorage &node_storage = NodeStorage::default_instance());
    [[nodiscard]] Node try_get_node(std::string_view label) const noexcept;
    [[nodiscard]] Node get_or_generate_node(std::string_view label, NodeStorage &node_storage = NodeStorage::default_instance());

    [[nodiscard]] NodeScope subscope(std::string scope_name) noexcept;

    bool operator==(NodeScope const &other) const noexcept = default;
    bool operator!=(NodeScope const &other) const noexcept = default;
};

}  //namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_RDF_UTIL_BLANKNODEIDSCOPE_HPP
