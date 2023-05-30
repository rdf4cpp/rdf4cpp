#ifndef RDF4CPP_NODEBACKENDHANDLE_HPP
#define RDF4CPP_NODEBACKENDHANDLE_HPP

#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/NodeStorageID.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/RDFNodeType.hpp>
#include <rdf4cpp/rdf/storage/node/view/BNodeBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/view/IRIBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/view/LiteralBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/view/VariableBackendView.hpp>


#include <compare>
#include <cstddef>

namespace rdf4cpp::rdf::storage::node::identifier {
/**
 * NodeBackendHandle is a a 64 bit wide type that is used to identify <dev>Node</dev>s stored at the backend.
 * It consists of a NodeID, a RDFNodeType, a NodeStorageID and free tagging bits.
 */
class NodeBackendHandle {
private:
    uint64_t raw_;

    explicit NodeBackendHandle(uint64_t raw) noexcept;

public:
    NodeBackendHandle() noexcept = default;

    /**
     * Constructor
     * @param node_id NodeID to be used. NodeID() indicates a null() NodeBackendHandle.
     * @param node_type RDFNodeType to be used
     * @param node_storage_id  the NodeStorageID
     * @param tagging_bits tagging bits (must be all zero for usage with the backend)
     */
    explicit NodeBackendHandle(NodeID node_id, RDFNodeType node_type, NodeStorageID node_storage_id, bool inlined = false, uint8_t tagging_bits = {}) noexcept;

    /**
     * Constructs a NodeBackendHandle from it's raw bit representation.
     * @param raw bit repr
     * @return NodeBackendHandle
     */
    static NodeBackendHandle from_raw(uint64_t raw) noexcept;

    /**
     * Get the RDFNodeType
     * @returnRDFNodeType
     */
    [[nodiscard]] RDFNodeType type() const noexcept;

    /**
     * Check if the NodeBackendHandle identifies a IRI.
     * @return
     */
    [[nodiscard]] bool is_iri() const noexcept;
    /**
     * Check if the NodeBackendHandle identifies a Literal.
     * @return
     */
    [[nodiscard]] bool is_literal() const noexcept;
    /**
     * Check if the NodeBackendHandle identifies a BlankNode.
     * @return
     */
    [[nodiscard]] bool is_blank_node() const noexcept;
    /**
     * Check if the NodeBackendHandle identifies a Variable.
     * @return
     */
    [[nodiscard]] bool is_variable() const noexcept;

    /**
     * If true, the NodeBackendHandle identifies no Node.
     * @return
     */
    [[nodiscard]] bool null() const noexcept;

    /**
     * Get the NodeStorageID
     * @return
     */
    [[nodiscard]] NodeStorageID node_storage_id() const noexcept;

    /**
     * Get the NodeID
     * @return
     */
    [[nodiscard]] NodeID node_id() const noexcept;

    /**
     * @return Whether the LiteralID is an inlined value or a normal node storage ID
     */
    [[nodiscard]] bool is_inlined() const noexcept;

    /**
     * Get the free tagging bits
     * @return
     */
    [[nodiscard]] uint8_t free_tagging_bits() const noexcept;

    /**
     * Set the free tagging bits. Must be reset to 0 before using comparison functions or raw() const
     * @param new_value
     */
    void set_free_tagging_bits(uint8_t new_value);

    /**
     * Retrieve underlying 64 bit data
     * @return
     */
    [[nodiscard]] uint64_t raw() const noexcept;

    auto operator<=>(NodeBackendHandle const &other) const noexcept = default;

    bool operator==(NodeBackendHandle const &other) const noexcept = default;

    /**
     * Retrieve IRIBackendView with information details about the IRI identified by this NodeBackendHandle.
     * MUST only be called if this is not null and is_iri() const is true.
     * @return
     */
    [[nodiscard]] view::IRIBackendView iri_backend() const noexcept;
    /**
     * Retrieve LiteralBackendView with information details about the Literal identified by this NodeBackendHandle.
     * MUST only be called if this is not null and is_literal() const is true.
     * @return
     */
    [[nodiscard]] view::LiteralBackendView literal_backend() const noexcept;
    /**
     * Retrieve BNodeBackendView with information details about the BlankNode identified by this NodeBackendHandle.
     * MUST only be called if this is not null and is_bnode() const is true.
     * @return
     */
    [[nodiscard]] view::BNodeBackendView bnode_backend() const noexcept;

    /**
     * Retrieve VariableBackendView with information details about the Variable identified by this NodeBackendHandle.
     * MUST only be called if this is not null and is_variable() const is true.
     * @return
     */
    [[nodiscard]] view::VariableBackendView variable_backend() const noexcept;
};
static_assert(sizeof(NodeBackendHandle) == sizeof(uint64_t));

/**
 * Constructor to create an IRI handle for the datatype of a Literal with fixed id datatype
 * @param lit_handle handle of the literal
 * @return handle to the datatype of the literal
 */
[[nodiscard]] NodeBackendHandle datatype_iri_handle_for_fixed_lit_handle(NodeBackendHandle lit_handle) noexcept;

}  // namespace rdf4cpp::rdf::storage::node::identifier


template<>
struct std::hash<rdf4cpp::rdf::storage::node::identifier::NodeBackendHandle> {
    inline size_t operator()(rdf4cpp::rdf::storage::node::identifier::NodeBackendHandle const &v) const noexcept {
        return v.raw();
    }
};

#endif  //RDF4CPP_NODEBACKENDHANDLE_HPP
