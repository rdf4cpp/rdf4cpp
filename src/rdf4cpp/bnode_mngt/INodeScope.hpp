#ifndef RDF4CPP_INODESCOPE_HPP
#define RDF4CPP_INODESCOPE_HPP

#include <rdf4cpp/storage/identifier/NodeBackendHandle.hpp>
#include <rdf4cpp/bnode_mngt/INodeFactory.hpp>

#include <optional>
#include <string_view>

namespace rdf4cpp::rdf::bnode_mngt {

/**
 * Base backend interface for NodeScopes.
 * The scope remembers label <-> node relationships. For example as in BlankNode label <-> actual BlankNode in the storage.
 */
struct INodeScope {
    virtual ~INodeScope() noexcept = default;

    /**
     * Try to find a node with the given label in this scope.
     *
     * @param label the label to search for
     * @return either the handle for the found node, or a null-handle if this scope does not have such a node
     */
    [[nodiscard]] virtual storage::identifier::NodeBackendHandle find_node(std::string_view label) const noexcept = 0;

    /**
     * Try to find the label corresponding to the given node handle-
     *
     * @param handle the node handle to search for
     * @return either the label for the given node, or std::nullopt if this scope does not know this handle
     */
    [[nodiscard]] virtual std::optional<std::string_view> find_label(storage::identifier::NodeBackendHandle handle) const noexcept = 0;

    /**
     * Insert a mapping label <-> handle into this scope
     *
     * @param label the label of the mapping
     * @param handle the handle of the mapping
     * @throws std::invalid_argument if the given node is already labeled
     *
     * @warning handle must be a handle to a valid node otherwise behaviour is undefined
     */
    virtual void label_node(std::string_view label, storage::identifier::NodeBackendHandle handle) = 0;
};

}  //namespace rdf4cpp::rdf::bnode_mngt

#endif  //RDF4CPP_INODESCOPE_HPP
