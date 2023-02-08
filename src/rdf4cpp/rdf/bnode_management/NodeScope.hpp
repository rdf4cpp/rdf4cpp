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

/**
 * A scope for generating Nodes that remembers the nodes it generates.
 * Uses an IIdGenerator and INodeScope internally to perform its function.
 *
 * Instances of this type cannot be constructed directly, use NodeGenerator::scope instead.
 */
struct NodeScope {
    using NodeStorage = storage::node::NodeStorage;

private:
    friend NodeGenerator;

    IIdGenerator *generator;
    SharedPtr<INodeScope> scope;

    explicit NodeScope(IIdGenerator *generator_backend, SharedPtr<INodeScope> scope_backend) noexcept;
public:
    NodeScope(NodeScope &&other) noexcept;

    /**
     * Generates a new node without remembering it. This behaves identically to NodeGenerator::generate_node
     * @param node_storage where to place the generated node
     * @return the generated node
     */
    [[nodiscard]] Node generate_node(NodeStorage &node_storage = NodeStorage::default_instance());

    /**
     * Try to fetch a previously generated (and remembered) node by using the previously used label.
     * @param label the label that was used initially to generate the searched-for node
     * @return the node if it was found or a null-node if this scope does not remember the given label
     */
    [[nodiscard]] Node try_get_node(std::string_view label) const noexcept;

    /**
     * Try to fetch a previously generated (and remembered) node by using the previously used label.
     * If the scope remembers no such label, a new node is generated and remembered.
     *
     * @param label the label of the previous node/the label of the new node that will be generated
     * @param node_storage where to place the newly generated node
     * @return either the found node or the newly generated node
     */
    [[nodiscard]] Node get_or_generate_node(std::string_view label, NodeStorage &node_storage = NodeStorage::default_instance());

    /**
     * Create a subscope of this scope. Subscopes remember mappings from their parents
     * but parents do not know the mappings of their children.
     * Repeated calls with the same scope name return the same scope.
     *
     * @param scope_name the name of the new subscope, could be a graph iri or some identifier identifying a subquery
     * @return a shared ptr to the new subscope
     */
    [[nodiscard]] NodeScope subscope(std::string scope_name) noexcept;

    /**
     * @return whether this and other refer to the same backends
     */
    bool operator==(NodeScope const &other) const noexcept = default;

    /**
     * @return whether this and other do _not_ refer to the same backends
     */
    bool operator!=(NodeScope const &other) const noexcept = default;
};

}  //namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_RDF_UTIL_BLANKNODEIDSCOPE_HPP
