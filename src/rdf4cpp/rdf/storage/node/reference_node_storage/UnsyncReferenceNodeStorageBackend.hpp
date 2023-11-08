#ifndef RDF4CPP_UNSYNCREFERENCENODESTORAGEBACKEND_HPP
#define RDF4CPP_UNSYNCREFERENCENODESTORAGEBACKEND_HPP

#include <cstdint>
#include <tuple>

#include <rdf4cpp/rdf/storage/node/reference_node_storage/detail/UnsyncNodeTypeStorage.hpp>
#include <rdf4cpp/rdf/storage/node/INodeStorageBackend.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/BNodeBackend.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/FallbackLiteralBackend.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/IRIBackend.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/SpecializedLiteralBackend.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/VariableBackend.hpp>


namespace rdf4cpp::rdf::storage::node::reference_node_storage {

/**
 * NON-Thread-safe reference implementation of a INodeStorageBackend.
 */
class UnsyncReferenceNodeStorageBackend : public INodeStorageBackend {
public:
    using NodeID = identifier::NodeID;
    using LiteralID = identifier::LiteralID;

private:
    UnsyncNodeTypeStorage<BNodeBackend> bnode_storage_;
    UnsyncNodeTypeStorage<IRIBackend> iri_storage_;
    UnsyncNodeTypeStorage<VariableBackend> variable_storage_;

    UnsyncNodeTypeStorage<FallbackLiteralBackend> fallback_literal_storage_;

    std::tuple<UnsyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::Integer>>,
               UnsyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::NonNegativeInteger>>,
               UnsyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::PositiveInteger>>,
               UnsyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::NonPositiveInteger>>,
               UnsyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::NegativeInteger>>,
               UnsyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::Long>>,
               UnsyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::UnsignedLong>>,

               UnsyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::Decimal>>,
               UnsyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::Double>>,

               UnsyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::Base64Binary>>,
               UnsyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::HexBinary>>,

               UnsyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::Date>>,
               UnsyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::DateTime>>,
               UnsyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::DateTimeStamp>>,
               UnsyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::GYearMonth>>,
               UnsyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::Duration>>,
               UnsyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::DayTimeDuration>>,
               UnsyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::YearMonthDuration>>> specialized_literal_storage_;

    uint64_t next_fallback_literal_id_{NodeID::min_literal_id.value};
    std::array<uint64_t, std::tuple_size_v<decltype(specialized_literal_storage_)>> next_specialized_literal_ids_;

    uint64_t next_bnode_id_{NodeID::min_bnode_id.value()};
    uint64_t next_iri_id_{NodeID::min_iri_id.value()};
    uint64_t next_variable_id_{NodeID::min_variable_id.value()};

public:
    UnsyncReferenceNodeStorageBackend() noexcept;

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

    bool erase_iri(identifier::NodeID id) noexcept override;
    bool erase_literal(identifier::NodeID id) noexcept override;
    bool erase_bnode(identifier::NodeID id) noexcept override;
    bool erase_variable(identifier::NodeID id) noexcept override;
};

}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage

#endif  //RDF4CPP_UNSYNCREFERENCENODESTORAGEBACKEND_HPP
