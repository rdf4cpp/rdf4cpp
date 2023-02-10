#ifndef RDF4CPP_REFERENCENODESTORAGEBACKEND_HPP
#define RDF4CPP_REFERENCENODESTORAGEBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/INodeStorageBackend.hpp>

#include <rdf4cpp/rdf/storage/node/reference_node_storage/NodeTypeStorage.hpp>

#include <rdf4cpp/rdf/storage/node/reference_node_storage/BNodeBackend.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/FallbackLiteralBackend.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/IRIBackend.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/SpecializedLiteralBackend.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/VariableBackend.hpp>


namespace rdf4cpp::rdf::storage::node::reference_node_storage {

/**
 * Thread-safe reference implementation of a INodeStorageBackend.
 */
class ReferenceNodeStorageBackend : public INodeStorageBackend {
public:
    using NodeID = identifier::NodeID;
    using LiteralID = identifier::LiteralID;

private:
    NodeTypeStorage<BNodeBackend> bnode_storage_;
    NodeTypeStorage<IRIBackend> iri_storage_;
    NodeTypeStorage<VariableBackend> variable_storage_;

    NodeTypeStorage<FallbackLiteralBackend> fallback_literal_storage_;

    NodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::Integer>> xsd_integer_literal_storage_;
    NodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::NonNegativeInteger>> xsd_non_negative_integer_literal_storage_;
    NodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::PositiveInteger>> xsd_positive_integer_literal_storage_;
    NodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::NonPositiveInteger>> xsd_non_positive_integer_literal_storage_;
    NodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::NegativeInteger>> xsd_negative_integer_literal_storage_;
    NodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::Long>> xsd_long_literal_storage_;
    NodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::UnsignedLong>> xsd_unsigned_long_literal_storage_;

    NodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::Decimal>> xsd_decimal_literal_storage_;
    NodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::Double>> xsd_double_literal_storage_;

    NodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::Base64Binary>> xsd_base64binary_literal_storage_;
    NodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::HexBinary>> xsd_hexbinary_literal_storage_;

    LiteralID next_literal_id = NodeID::min_literal_id;
    NodeID next_bnode_id = NodeID::min_bnode_id;
    NodeID next_iri_id = NodeID::min_iri_id;
    NodeID next_variable_id = NodeID::min_variable_id;

    template<typename Self, typename F>
    friend auto visit_specialized(Self &&self, identifier::LiteralType datatype, F f);

public:
    ReferenceNodeStorageBackend() noexcept;

    [[nodiscard]] size_t size() const noexcept override;
    [[nodiscard]] bool has_specialized_storage_for(identifier::LiteralType datatype) const noexcept override;

    [[nodiscard]] identifier::NodeID find_or_make_id(view::BNodeBackendView const &view) noexcept override;
    [[nodiscard]] identifier::NodeID find_or_make_id(view::IRIBackendView const &view) noexcept override;
    [[nodiscard]] identifier::NodeID find_or_make_id(view::LiteralBackendView const &view) noexcept override;
    [[nodiscard]] identifier::NodeID find_or_make_id(view::VariableBackendView const &view) noexcept override;

    [[nodiscard]] identifier::NodeID find_id(view::BNodeBackendView const &view) const noexcept override;
    [[nodiscard]] identifier::NodeID find_id(view::IRIBackendView const &view) const noexcept override;
    [[nodiscard]] identifier::NodeID find_id(view::LiteralBackendView const &view) const noexcept override;
    [[nodiscard]] identifier::NodeID find_id(view::VariableBackendView const &view) const noexcept override;

    [[nodiscard]] view::IRIBackendView find_iri_backend_view(identifier::NodeID id) const override;
    [[nodiscard]] view::LiteralBackendView find_literal_backend_view(identifier::NodeID id) const override;
    [[nodiscard]] view::BNodeBackendView find_bnode_backend_view(identifier::NodeID id) const override;
    [[nodiscard]] view::VariableBackendView find_variable_backend_view(identifier::NodeID id) const override;

    bool erase_iri(identifier::NodeID id) override;
    bool erase_literal(identifier::NodeID id) override;
    bool erase_bnode(identifier::NodeID id) override;
    bool erase_variable(identifier::NodeID id) override;
};

}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage
#endif  //RDF4CPP_REFERENCENODESTORAGEBACKEND_HPP
