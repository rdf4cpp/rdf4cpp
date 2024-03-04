#ifndef RDF4CPP_NODEBACKENDHANDLE_HPP
#define RDF4CPP_NODEBACKENDHANDLE_HPP

#include <rdf4cpp/rdf/storage/node/NodeStorage.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/NodeStorageID.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/RDFNodeType.hpp>
#include <rdf4cpp/rdf/storage/node/view/BNodeBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/view/IRIBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/view/LiteralBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/view/VariableBackendView.hpp>

#include <compare>
#include <cstdint>

namespace rdf4cpp::rdf::storage::node::identifier {
/**
 * NodeBackendHandle is a a 64 bit wide type that is used to identify \ref Node "Nodes" stored at the backend.
 * It consists of a NodeID, a RDFNodeType, a NodeStorageID and free tagging bits.
 */
struct NodeBackendHandle {
    using underlying_id_type = uint64_t;
    static constexpr size_t id_width = 64;

private:
    struct __attribute__((__packed__)) id_parts {
        NodeID node_id_;
        RDFNodeType node_type_: 2;
        uint8_t inlined_: 1;
        uint16_t free_tagging_bits_: 13;

        static_assert(NodeID::width + 2 + 1 + 13 == 64);
    };

    union __attribute__((packed)) id_type {
        underlying_id_type underlying_: id_width;
        id_parts parts_;
    };

    id_type id_;
    DynNodeStorage storage_;


public:
    NodeBackendHandle() noexcept = default;

    NodeBackendHandle(underlying_id_type const underlying_id,
                      DynNodeStorage storage) noexcept : id_{.underlying_ = underlying_id},
                                                         storage_{storage} {
    }

    /**
     * Constructor
     * @param node_id NodeID to be used. NodeID() indicates a null() NodeBackendHandle.
     * @param node_type RDFNodeType to be used
     * @param inlined inlined bit
     * @param tagging_bits tagging bits (must be all zero for usage with the backend)
     */
    NodeBackendHandle(NodeID const node_id,
                      RDFNodeType const node_type,
                      DynNodeStorage storage,
                      bool const inlined = false,
                      uint16_t const tagging_bits = 0) noexcept : id_{.parts_{node_id, node_type, inlined, tagging_bits}},
                      storage_{storage} {
        assert(tagging_bits < (1 << 13));
    }

    /**
     * Get the RDFNodeType
     * @return RDFNodeType
     */
    [[nodiscard]] constexpr RDFNodeType type() const noexcept {
        return id_.parts_.node_type_;
    }

    /**
     * Check if the NodeBackendHandle identifies a IRI.
     * @return
     */
    [[nodiscard]] constexpr bool is_iri() const noexcept {
        return id_.parts_.node_type_ == RDFNodeType::IRI;
    }

    /**
     * Check if the NodeBackendHandle identifies a Literal.
     * @return
     */
    [[nodiscard]] constexpr bool is_literal() const noexcept {
        return id_.parts_.node_type_ == RDFNodeType::Literal;
    }

    /**
     * Check if the NodeBackendHandle identifies a BlankNode.
     * @return
     */
    [[nodiscard]] constexpr bool is_blank_node() const noexcept {
        return id_.parts_.node_type_ == RDFNodeType::BNode;
    }

    /**
     * Check if the NodeBackendHandle identifies a Variable.
     * @return
     */
    [[nodiscard]] constexpr bool is_variable() const noexcept {
        return id_.parts_.node_type_ == RDFNodeType::Variable;
    }

    /**
     * If true, the NodeBackendHandle identifies no Node.
     * @return
     */
    [[nodiscard]] constexpr bool null() const noexcept {
        return id_.parts_.node_id_.null();
    }

    /**
     * Get the NodeID
     * @return
     */
    [[nodiscard]] constexpr NodeID node_id() const noexcept {
        return id_.parts_.node_id_;
    }

    /**
     * @return Whether the LiteralID is an inlined value or a normal node storage ID
     */
    [[nodiscard]] constexpr bool is_inlined() const noexcept {
        return id_.parts_.inlined_;
    }

    /**
     * Get the free tagging bits
     * @return
     */
    [[nodiscard]] constexpr uint16_t free_tagging_bits() const noexcept {
        return id_.parts_.free_tagging_bits_;
    }

    /**
     * Set the free tagging bits. Must be reset to 0 before using comparison functions or raw() const
     * @param new_value
     */
    constexpr void set_free_tagging_bits(uint16_t new_value) noexcept {
        assert(new_value < (1 << 13));
        id_.parts_.free_tagging_bits_ = new_value;
    }

    /**
     * Retrieve underlying 64 bit data
     * @return
     */
    [[nodiscard]] underlying_id_type id_to_underlying() const noexcept {
        return id_.underlying_;
    }

    [[nodiscard]] DynNodeStorage storage() const noexcept {
        return storage_;
    }

    constexpr std::strong_ordering operator<=>(NodeBackendHandle const &other) const noexcept {
        return std::tie(id_.underlying_, storage_) <=> std::tie(other.id_.underlying_, other.storage_);
    }

    constexpr bool operator==(NodeBackendHandle const &other) const noexcept {
        return std::tie(id_.underlying_, storage_) == std::tie(other.id_.underlying_, other.storage_);
    }

    /**
     * Retrieve IRIBackendView with information details about the IRI identified by this NodeBackendHandle.
     * MUST only be called if this is not null and is_iri() const is true.
     * @return
     */
    [[nodiscard]] view::IRIBackendView iri_backend() const noexcept {
        assert(id_.parts_.node_type_ == RDFNodeType::IRI);
        return storage_.find_iri_backend(id_.parts_.node_id_);
    }

    /**
     * Retrieve LiteralBackendView with information details about the Literal identified by this NodeBackendHandle.
     * MUST only be called if this is not null and is_literal() const is true.
     * @return
     */
    [[nodiscard]] view::LiteralBackendView literal_backend() const noexcept {
        assert(id_.parts_.node_type_ == RDFNodeType::Literal);
        return storage_.find_literal_backend(id_.parts_.node_id_);
    }

    /**
     * Retrieve BNodeBackendView with information details about the BlankNode identified by this NodeBackendHandle.
     * MUST only be called if this is not null and is_bnode() const is true.
     * @return
     */
    [[nodiscard]] view::BNodeBackendView bnode_backend() const noexcept {
        assert(id_.parts_.node_type_ == RDFNodeType::BNode);
        return storage_.find_bnode_backend(id_.parts_.node_id_);
    }

    /**
     * Retrieve VariableBackendView with information details about the Variable identified by this NodeBackendHandle.
     * MUST only be called if this is not null and is_variable() const is true.
     * @return
     */
    [[nodiscard]] view::VariableBackendView variable_backend() const noexcept {
        assert(id_.parts_.node_type_ == RDFNodeType::Variable);
        return storage_.find_variable_backend(id_.parts_.node_id_);
    }
};

static_assert(sizeof(NodeBackendHandle) == 3 * sizeof(void *));

/**
 * Constructor to create an IRI handle for the datatype of a Literal with fixed id datatype
 * @param lit_handle handle of the literal
 * @return handle to the datatype of the literal
 */
[[nodiscard]] inline NodeBackendHandle datatype_iri_handle_for_fixed_lit_handle(NodeBackendHandle lit_handle) noexcept {
    assert(lit_handle.is_literal());
    assert(lit_handle.node_id().literal_type().is_fixed());
    return NodeBackendHandle{literal_type_to_iri_node_id(lit_handle.node_id().literal_type()),
                             storage::node::identifier::RDFNodeType::IRI,
                             lit_handle.storage()};
}

}  // namespace rdf4cpp::rdf::storage::node::identifier


template<typename Policy>
struct dice::hash::dice_hash_overload<Policy, rdf4cpp::rdf::storage::node::identifier::NodeBackendHandle> {
    static inline size_t dice_hash(rdf4cpp::rdf::storage::node::identifier::NodeBackendHandle const &x) noexcept {
        return dice::hash::dice_hash_templates<Policy>::dice_hash(std::make_tuple(x.id_to_underlying(), x.storage()));
    }
};

template<>
struct std::hash<rdf4cpp::rdf::storage::node::identifier::NodeBackendHandle> {
    inline size_t operator()(rdf4cpp::rdf::storage::node::identifier::NodeBackendHandle const &v) const noexcept {
        return ::dice::hash::dice_hash_templates<::dice::hash::Policies::wyhash>::dice_hash(v);
    }
};

#endif  //RDF4CPP_NODEBACKENDHANDLE_HPP
