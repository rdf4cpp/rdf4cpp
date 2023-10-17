#ifndef RDF4CPP_REFERENCENODESTORAGEBACKEND_HPP
#define RDF4CPP_REFERENCENODESTORAGEBACKEND_HPP

#include <atomic>
#include <tuple>

#include <rdf4cpp/rdf/datatypes/xsd.hpp>
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

    std::tuple<NodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::Integer>>,
               NodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::NonNegativeInteger>>,
               NodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::PositiveInteger>>,
               NodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::NonPositiveInteger>>,
               NodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::NegativeInteger>>,
               NodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::Long>>,
               NodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::UnsignedLong>>,

               NodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::Decimal>>,
               NodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::Double>>,

               NodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::Base64Binary>>,
               NodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::HexBinary>>,

               NodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::Date>>,
               NodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::DateTime>>,
               NodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::DateTimeStamp>>,
               NodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::GYearMonth>>,
               NodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::Duration>>,
               NodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::DayTimeDuration>>,
               NodeTypeStorage<SpecializedLiteralBackend<datatypes::xsd::YearMonthDuration>>> specialized_literal_storage_;

    std::atomic<uint64_t> next_fallback_literal_id_{NodeID::min_literal_id.value};
    std::array<std::atomic<uint64_t>, std::tuple_size_v<decltype(specialized_literal_storage_)>> next_specialized_literal_ids_;

    std::atomic<uint64_t> next_bnode_id_{NodeID::min_bnode_id.value()};
    std::atomic<uint64_t> next_iri_id_{NodeID::min_iri_id.value()};
    std::atomic<uint64_t> next_variable_id_{NodeID::min_variable_id.value()};

    /**
     * Calls the given function f with the specialized object for the given datatype
     *
     * @param self any container for specialized things in the correct order (i.e. specialized_literal_storage_ or next_specialized_literal_ids_)
     * @param datatype the datatype of the specialized object
     * @param f the function to call with the corresponding specialized object
     * @return whatever f returns
     */
    template<typename S, typename F>
    static decltype(auto) visit_specialized(S &&container, identifier::LiteralType datatype, F f);

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

    bool erase_iri(identifier::NodeID id) noexcept override;
    bool erase_literal(identifier::NodeID id) noexcept override;
    bool erase_bnode(identifier::NodeID id) noexcept override;
    bool erase_variable(identifier::NodeID id) noexcept override;
};

}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage
#endif  //RDF4CPP_REFERENCENODESTORAGEBACKEND_HPP
