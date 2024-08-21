#ifndef RDF4CPP_UNSYNCREFERENCENODESTORAGE_HPP
#define RDF4CPP_UNSYNCREFERENCENODESTORAGE_HPP

#include <cstddef>
#include <tuple>

#include <rdf4cpp/storage/NodeStorage.hpp>
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
struct UnsyncReferenceNodeStorage {
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

    void init();

public:
    UnsyncReferenceNodeStorage();

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

    void clear() noexcept;
};

static_assert(NodeStorage<UnsyncReferenceNodeStorage>);

}  // namespace rdf4cpp::storage::reference_node_storage

#endif  //RDF4CPP_UNSYNCREFERENCENODESTORAGE_HPP
