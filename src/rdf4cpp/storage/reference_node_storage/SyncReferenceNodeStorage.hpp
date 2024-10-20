#ifndef RDF4CPP_SYNCREFERENCENODESTORAGE_HPP
#define RDF4CPP_SYNCREFERENCENODESTORAGE_HPP

#include <tuple>

#include <rdf4cpp/storage/NodeStorage.hpp>
#include <rdf4cpp/storage/reference_node_storage/BNodeBackend.hpp>
#include <rdf4cpp/storage/reference_node_storage/FallbackLiteralBackend.hpp>
#include <rdf4cpp/storage/reference_node_storage/IRIBackend.hpp>
#include <rdf4cpp/storage/reference_node_storage/SpecializedLiteralBackend.hpp>
#include <rdf4cpp/storage/reference_node_storage/VariableBackend.hpp>
#include <rdf4cpp/storage/reference_node_storage/detail/SyncNodeTypeStorage.hpp>

namespace rdf4cpp::storage::reference_node_storage {

/**
 * Thread-safe reference implementation of a INodeStorageBackend.
 */
struct SyncReferenceNodeStorage {
private:
    SyncNodeTypeStorage<BNodeBackend> bnode_storage_;
    SyncNodeTypeStorage<IRIBackend> iri_storage_;
    SyncNodeTypeStorage<VariableBackend> variable_storage_;

    SyncNodeTypeStorage<FallbackLiteralBackend> fallback_literal_storage_;

    std::tuple<SyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::Integer>>,
               SyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::NonNegativeInteger>>,
               SyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::PositiveInteger>>,
               SyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::NonPositiveInteger>>,
               SyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::NegativeInteger>>,
               SyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::Long>>,
               SyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::UnsignedLong>>,

               SyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::Decimal>>,
               SyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::Double>>,

               SyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::Base64Binary>>,
               SyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::HexBinary>>,

               SyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::Date>>,
               SyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::DateTime>>,
               SyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::DateTimeStamp>>,
               SyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::GYearMonth>>,
               SyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::Duration>>,
               SyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::DayTimeDuration>>,
               SyncNodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::YearMonthDuration>>> specialized_literal_storage_;

public:
    SyncReferenceNodeStorage() noexcept;

    [[nodiscard]] size_t size() const noexcept;
    void shrink_to_fit();

    [[nodiscard]] static bool has_specialized_storage_for(identifier::LiteralType datatype) noexcept;

    [[nodiscard]] identifier::NodeBackendID find_or_make_id(view::BNodeBackendView const &view);
    [[nodiscard]] identifier::NodeBackendID find_or_make_id(view::IRIBackendView const &view);
    [[nodiscard]] identifier::NodeBackendID find_or_make_id(view::LiteralBackendView const &view);
    [[nodiscard]] identifier::NodeBackendID find_or_make_id(view::VariableBackendView const &view);

    [[nodiscard]] identifier::NodeBackendID find_id(view::BNodeBackendView const &view) const noexcept;
    [[nodiscard]] identifier::NodeBackendID find_id(view::IRIBackendView const &view) const noexcept;
    [[nodiscard]] identifier::NodeBackendID find_id(view::LiteralBackendView const &view) const noexcept;
    [[nodiscard]] identifier::NodeBackendID find_id(view::VariableBackendView const &view) const noexcept;

    [[nodiscard]] view::IRIBackendView find_iri_backend(identifier::NodeBackendID id) const noexcept;
    [[nodiscard]] view::LiteralBackendView find_literal_backend(identifier::NodeBackendID id) const noexcept;
    [[nodiscard]] view::BNodeBackendView find_bnode_backend(identifier::NodeBackendID id) const noexcept;
    [[nodiscard]] view::VariableBackendView find_variable_backend(identifier::NodeBackendID id) const noexcept;

    bool erase_iri(identifier::NodeBackendID id);
    bool erase_literal(identifier::NodeBackendID id);
    bool erase_bnode(identifier::NodeBackendID id);
    bool erase_variable(identifier::NodeBackendID id);
};
static_assert(NodeStorage<SyncReferenceNodeStorage>);

extern SyncReferenceNodeStorage default_instance;

}  // namespace rdf4cpp::storage::reference_node_storage
#endif  //RDF4CPP_SYNCREFERENCENODESTORAGE_HPP
