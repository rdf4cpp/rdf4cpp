#ifndef RDF4CPP_UNSYNCREFERENCENODESTORAGEBACKEND_HPP
#define RDF4CPP_UNSYNCREFERENCENODESTORAGEBACKEND_HPP

#include <cstdint>
#include <tuple>

#include <rdf4cpp/storage/reference_node_storage/BNodeBackend.hpp>
#include <rdf4cpp/storage/reference_node_storage/FallbackLiteralBackend.hpp>
#include <rdf4cpp/storage/reference_node_storage/IRIBackend.hpp>
#include <rdf4cpp/storage/reference_node_storage/SpecializedLiteralBackend.hpp>
#include <rdf4cpp/storage/reference_node_storage/VariableBackend.hpp>
#include <rdf4cpp/storage/reference_node_storage/detail/UnsyncNodeTypeStorage.hpp>

namespace rdf4cpp::storage::reference_node_storage {

/**
 * NON-Thread-safe reference implementation of a INodeStorageBackend.
 */
struct UnsyncReferenceNodeStorageBackend {
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

public:
    UnsyncReferenceNodeStorageBackend() noexcept;

    [[nodiscard]] size_t size() const noexcept;
    void shrink_to_fit();

    [[nodiscard]] static bool has_specialized_storage_for(identifier::LiteralType datatype) noexcept;

    [[nodiscard]] identifier::NodeID find_or_make_id(view::BNodeBackendView const &view) noexcept;
    [[nodiscard]] identifier::NodeID find_or_make_id(view::IRIBackendView const &view) noexcept;
    [[nodiscard]] identifier::NodeID find_or_make_id(view::LiteralBackendView const &view) noexcept;
    [[nodiscard]] identifier::NodeID find_or_make_id(view::VariableBackendView const &view) noexcept;

    [[nodiscard]] identifier::NodeID find_id(view::BNodeBackendView const &view) const noexcept;
    [[nodiscard]] identifier::NodeID find_id(view::IRIBackendView const &view) const noexcept;
    [[nodiscard]] identifier::NodeID find_id(view::LiteralBackendView const &view) const noexcept;
    [[nodiscard]] identifier::NodeID find_id(view::VariableBackendView const &view) const noexcept;

    [[nodiscard]] view::IRIBackendView find_iri_backend_view(identifier::NodeID id) const;
    [[nodiscard]] view::LiteralBackendView find_literal_backend_view(identifier::NodeID id) const;
    [[nodiscard]] view::BNodeBackendView find_bnode_backend_view(identifier::NodeID id) const;
    [[nodiscard]] view::VariableBackendView find_variable_backend_view(identifier::NodeID id) const;

    bool erase_iri(identifier::NodeID id) noexcept;
    bool erase_literal(identifier::NodeID id) noexcept;
    bool erase_bnode(identifier::NodeID id) noexcept;
    bool erase_variable(identifier::NodeID id) noexcept;

    void clear() noexcept;
};

}  // namespace rdf4cpp::storage::reference_node_storage

#endif  //RDF4CPP_UNSYNCREFERENCENODESTORAGEBACKEND_HPP
