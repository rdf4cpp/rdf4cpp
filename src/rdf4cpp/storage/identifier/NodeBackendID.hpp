#ifndef RDF4CPP_NODEBACKENDID_HPP
#define RDF4CPP_NODEBACKENDID_HPP

#include <rdf4cpp/storage/identifier/NodeID.hpp>
#include <rdf4cpp/storage/identifier/RDFNodeType.hpp>

namespace rdf4cpp::storage::identifier {

struct alignas(void *) NodeBackendID {
    using underlying_type = uint64_t;
    static constexpr size_t width = 64;

private:
    struct __attribute__((__packed__)) id_parts {
        NodeID node_id_;
        RDFNodeType node_type_: 2;
        uint8_t inlined_: 1;
        uint16_t free_tagging_bits_: 13;

        static_assert(NodeID::width + 2 + 1 + 13 == width);
    };

    union __attribute__((packed)) {
        underlying_type underlying_: width;
        id_parts parts_;
    };

public:
    NodeBackendID() noexcept = default;

    explicit NodeBackendID(underlying_type const underlying_id) noexcept : underlying_{underlying_id} {
    }

    /**
     * Constructor
     * @param node_id NodeID to be used. NodeID() indicates a null() NodeBackendHandle.
     * @param node_type RDFNodeType to be used
     * @param inlined inlined bit
     * @param tagging_bits tagging bits (must be all zero for usage with the backend)
     */
    NodeBackendID(NodeID const node_id,
                  RDFNodeType const node_type,
                  bool const inlined = false,
                  uint16_t const tagging_bits = 0) noexcept : parts_{node_id, node_type, inlined, tagging_bits} {
        assert(tagging_bits < (1 << 13));
    }

    /**
     * Get the RDFNodeType
     * @return RDFNodeType
     */
    [[nodiscard]] constexpr RDFNodeType type() const noexcept {
        return parts_.node_type_;
    }

    /**
     * Check if the NodeBackendHandle identifies a IRI.
     * @return
     */
    [[nodiscard]] constexpr bool is_iri() const noexcept {
        return parts_.node_type_ == RDFNodeType::IRI;
    }

    /**
     * Check if the NodeBackendHandle identifies a Literal.
     * @return
     */
    [[nodiscard]] constexpr bool is_literal() const noexcept {
        return parts_.node_type_ == RDFNodeType::Literal;
    }

    /**
     * Check if the NodeBackendHandle identifies a BlankNode.
     * @return
     */
    [[nodiscard]] constexpr bool is_blank_node() const noexcept {
        return parts_.node_type_ == RDFNodeType::BNode;
    }

    /**
     * Check if the NodeBackendHandle identifies a Variable.
     * @return
     */
    [[nodiscard]] constexpr bool is_variable() const noexcept {
        return parts_.node_type_ == RDFNodeType::Variable;
    }

    /**
     * If true, the NodeBackendHandle identifies no Node.
     * @return
     */
    [[nodiscard]] constexpr bool null() const noexcept {
        return parts_.node_id_.null();
    }

    /**
     * Get the NodeID
     * @return
     */
    [[nodiscard]] constexpr NodeID node_id() const noexcept {
        return parts_.node_id_;
    }

    /**
     * @return Whether the LiteralID is an inlined value or a normal node storage ID
     */
    [[nodiscard]] constexpr bool is_inlined() const noexcept {
        return parts_.inlined_;
    }

    /**
     * Get the free tagging bits
     * @return
     */
    [[nodiscard]] constexpr uint16_t free_tagging_bits() const noexcept {
        return parts_.free_tagging_bits_;
    }

    /**
     * Set the free tagging bits. Must be reset to 0 before using comparison functions or raw() const
     * @param new_value
     */
    constexpr void set_free_tagging_bits(uint16_t new_value) noexcept {
        assert(new_value < (1 << 13));
        parts_.free_tagging_bits_ = new_value;
    }

    /**
     * Retrieve underlying 64 bit data
     * @return
     */
    [[nodiscard]] underlying_type to_underlying() const noexcept {
        return underlying_;
    }

    constexpr std::strong_ordering operator<=>(NodeBackendID const &other) const noexcept {
        return underlying_ <=> other.underlying_;
    }

    constexpr bool operator==(NodeBackendID const &other) const noexcept {
        return underlying_ == other.underlying_;
    }
};

static_assert(sizeof(NodeBackendID) == sizeof(void *));
static_assert(alignof(NodeBackendID) == alignof(void *));

} // namespace rdf4cpp::storage::identifier

template<typename Policy>
struct dice::hash::dice_hash_overload<Policy, rdf4cpp::storage::identifier::NodeBackendID> {
    static inline size_t dice_hash(rdf4cpp::storage::identifier::NodeBackendID const &x) noexcept {
        return dice::hash::dice_hash_templates<Policy>::dice_hash(x.to_underlying());
    }
};

template<>
struct std::hash<rdf4cpp::storage::identifier::NodeBackendID> {
    inline size_t operator()(rdf4cpp::storage::identifier::NodeBackendID const &v) const noexcept {
        return ::dice::hash::dice_hash_templates<::dice::hash::Policies::wyhash>::dice_hash(v);
    }
};

#endif // RDF4CPP_NODEBACKENDID_HPP
