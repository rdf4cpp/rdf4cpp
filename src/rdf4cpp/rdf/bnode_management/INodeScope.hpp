#ifndef RDF4CPP_INODESCOPE_HPP
#define RDF4CPP_INODESCOPE_HPP

#include <rdf4cpp/rdf/bnode_management/INodeFactory.hpp>
#include <rdf4cpp/rdf/util/SharedPtr.hpp>

#include <optional>
#include <string_view>

namespace rdf4cpp::rdf::util {

/**
 * Base backend interface for NodeScopes.
 * The scope remembers label <-> node relationships. For example as in BlankNode label <-> actual BlankNode in the storage.
 */
struct INodeScope : virtual INodeFactory {
    /**
     * Try to find a node with the given label in this scope.
     *
     * @param label the label to search for
     * @return either the handle for the found node, or a null-handle if this scope does not have such a node
     */
    [[nodiscard]] virtual storage::node::identifier::NodeBackendHandle find_node(std::string_view label) const noexcept = 0;

    /**
     * Try to find the label corresponding to the given node handle-
     *
     * @param handle the node handle to search for
     * @return either the label for the given node, or std::nullopt if this scope does not know this handle
     */
    [[nodiscard]] virtual std::optional<std::string_view> find_label(storage::node::identifier::NodeBackendHandle handle) const noexcept = 0;

    /**
     * Insert a mapping label <-> handle into this scope
     *
     * @param label the label of the mapping
     * @param handle the handle of the mapping
     */
    virtual void label_node(std::string_view label, storage::node::identifier::NodeBackendHandle handle) = 0;

    /**
     * Create a subscope of this scope. Subscopes should remember mappings from their parents
     * but parents do not know the mappings of their children.
     * Repeated calls with the same scope name should return the same scope.
     *
     * @param scope_name the name of the new subscope, could be a graph iri or some identifier identifying a subquery
     * @return a shared ptr to the new subscope
     */
    [[nodiscard]] virtual SharedPtr<INodeScope> subscope(std::string scope_name) noexcept = 0;
};

}  //namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_INODESCOPE_HPP
