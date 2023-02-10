#include "ReferenceNodeStorageBackend.hpp"

#include <array>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

static constexpr std::array specialization_list{
        datatypes::xsd::Integer::fixed_id,
        datatypes::xsd::NonNegativeInteger::fixed_id,
        datatypes::xsd::PositiveInteger::fixed_id,
        datatypes::xsd::NonPositiveInteger::fixed_id,
        datatypes::xsd::NegativeInteger::fixed_id,
        datatypes::xsd::Long::fixed_id,
        datatypes::xsd::UnsignedLong::fixed_id,
        datatypes::xsd::Decimal::fixed_id,
        datatypes::xsd::Double::fixed_id,
        datatypes::xsd::Base64Binary::fixed_id,
        datatypes::xsd::HexBinary::fixed_id};

namespace specialization_detail {
template<size_t N>
static consteval std::array<bool, 1 << identifier::LiteralType::width> make_storage_specialization_lut(std::array<identifier::LiteralType, N> const &specialized) noexcept {
    std::array<bool, 1 << identifier::LiteralType::width> ret{};
    for (auto const type : specialized) {
        ret[type.to_underlying()] = true;
    }

    return ret;
}

static constexpr std::array<bool, 1 << identifier::LiteralType::width> specialization_lut = specialization_detail::make_storage_specialization_lut(specialization_list);
}  //specialization_detail

template<typename Self, typename F>
auto visit_specialized(Self &&self, identifier::LiteralType const datatype, F f) {
    using namespace rdf4cpp::rdf::datatypes;

    assert(self.has_specialized_storage_for(datatype));

    switch (datatype.to_underlying()) {
        case xsd::Integer::fixed_id.to_underlying():
            return f(self.xsd_integer_literal_storage_);
        case xsd::NonNegativeInteger::fixed_id.to_underlying():
            return f(self.xsd_non_negative_integer_literal_storage_);
        case xsd::PositiveInteger::fixed_id.to_underlying():
            return f(self.xsd_positive_integer_literal_storage_);
        case xsd::NonPositiveInteger::fixed_id.to_underlying():
            return f(self.xsd_non_positive_integer_literal_storage_);
        case xsd::NegativeInteger::fixed_id.to_underlying():
            return f(self.xsd_negative_integer_literal_storage_);
        case xsd::Long::fixed_id.to_underlying():
            return f(self.xsd_long_literal_storage_);
        case xsd::UnsignedLong::fixed_id.to_underlying():
            return f(self.xsd_unsigned_long_literal_storage_);
        case xsd::Decimal::fixed_id.to_underlying():
            return f(self.xsd_decimal_literal_storage_);
        case xsd::Double::fixed_id.to_underlying():
            return f(self.xsd_double_literal_storage_);
        case xsd::Base64Binary::fixed_id.to_underlying():
            return f(self.xsd_base64binary_literal_storage_);
        case xsd::HexBinary::fixed_id.to_underlying():
            return f(self.xsd_hexbinary_literal_storage_);
        default:
            assert(false);
            __builtin_unreachable();
    }
}

ReferenceNodeStorageBackend::ReferenceNodeStorageBackend() noexcept {
    // some iri's like xsd:string are there by default

    for (const auto &[iri, literal_type] : datatypes::registry::reserved_datatype_ids) {
        auto const id = literal_type.to_underlying();

        auto const [it, inserted] = iri_storage_.id2data.emplace(id, std::make_unique<IRIBackend>(iri));
        assert(inserted);
        iri_storage_.data2id.emplace(it->second.get(), id);
    }
}

size_t ReferenceNodeStorageBackend::size() const noexcept {
    return iri_storage_.id2data.size() +
           bnode_storage_.id2data.size() +
           variable_storage_.id2data.size() +
           fallback_literal_storage_.id2data.size() +
           xsd_integer_literal_storage_.id2data.size() +
           xsd_non_negative_integer_literal_storage_.id2data.size() +
           xsd_positive_integer_literal_storage_.id2data.size() +
           xsd_non_positive_integer_literal_storage_.id2data.size() +
           xsd_negative_integer_literal_storage_.id2data.size() +
           xsd_long_literal_storage_.id2data.size() +
           xsd_unsigned_long_literal_storage_.id2data.size() +
           xsd_decimal_literal_storage_.id2data.size() +
           xsd_double_literal_storage_.id2data.size() +
           xsd_base64binary_literal_storage_.id2data.size() +
           xsd_hexbinary_literal_storage_.id2data.size();
}

bool ReferenceNodeStorageBackend::has_specialized_storage_for(identifier::LiteralType const datatype) const noexcept {
    return specialization_detail::specialization_lut[datatype.to_underlying()];
}

/**
 * Synchronized lookup (and creation) of IDs by a provided view of a Node Backend.
 * @tparam Backend_t the Backend type. One of BNodeBackend, IRIBackend, FallbackLiteralBackend or VariableBackend
 * @tparam create_if_not_present enables code for creating non-existing Node Backends
 * @tparam NextIDFromView_func type of a function to generate the next ID which is assigned in case a new Node Backend is created
 * @param view contains the data of the requested Node Backend
 * @param storage the storage where the Node Backend is looked up
 * @param next_id_func function to generate the next ID which is assigned in case a new Node Backend is created
 * @return the NodeID for the looked up Node Backend. Result is null() if there was no matching Node Backend.
 */
template<bool create_if_not_present, typename Storage, typename NextIDFunc = void *>
    requires (!create_if_not_present || std::is_nothrow_invocable_r_v<identifier::NodeID, NextIDFunc>)
static identifier::NodeID lookup_or_insert_impl(typename Storage::BackendView const &view,
                                                Storage &storage,
                                                NextIDFunc next_id_func = nullptr) noexcept {

    {
        std::shared_lock lock{storage.mutex};
        if (auto const it = storage.data2id.find(view); it != storage.data2id.end()) {
            return it->second;
        }
    }

    if constexpr (!create_if_not_present) {
        return identifier::NodeID{};
    } else {
        std::unique_lock lock{storage.mutex};

        // check again, might have changed between unlocking of shared_lock and locking of unique_lock
        if (auto const it = storage.data2id.find(view); it != storage.data2id.end()) {
            return it->second;
        }

        identifier::NodeID const next_id = next_id_func();
        auto const [it, inserted] = storage.id2data.emplace(next_id, std::make_unique<typename Storage::Backend>(view));
        assert(inserted);
        storage.data2id.emplace(it->second.get(), next_id);

        return next_id;
    }
}

identifier::NodeID ReferenceNodeStorageBackend::find_or_make_id(view::LiteralBackendView const &view) noexcept {
    return visit(view,
            [this](view::LexicalFormLiteralBackendView const &lexical) noexcept {
                auto const next_id_func = [this, datatype = lexical.datatype_id]() noexcept {
                    return identifier::NodeID{next_literal_id++, identifier::iri_node_id_to_literal_type(datatype)};
                };

                return lookup_or_insert_impl<true>(lexical, fallback_literal_storage_, next_id_func);
            },
            [this](view::ValueLiteralBackendView const &any) noexcept {
                auto const next_id_func = [this, datatype = any.datatype]() noexcept {
                    return identifier::NodeID{next_literal_id++, datatype};
                };

                return visit_specialized(*this, any.datatype, [&](auto &storage) {
                    return lookup_or_insert_impl<true>(any, storage, next_id_func);
                });
            });
}

identifier::NodeID ReferenceNodeStorageBackend::find_or_make_id(view::IRIBackendView const &view) noexcept {
    return lookup_or_insert_impl<true>(
            view, iri_storage_,
            [this]() noexcept {
                return next_iri_id++;
            });
}

identifier::NodeID ReferenceNodeStorageBackend::find_or_make_id(view::BNodeBackendView const &view) noexcept {
    return lookup_or_insert_impl<true>(
            view, bnode_storage_,
            [this]() noexcept {
                return next_bnode_id++;
            });
}

identifier::NodeID ReferenceNodeStorageBackend::find_or_make_id(view::VariableBackendView const &view) noexcept {
    return lookup_or_insert_impl<true>(
            view, variable_storage_,
            [this]() noexcept {
                return next_variable_id++;
            });
}

identifier::NodeID ReferenceNodeStorageBackend::find_id(view::BNodeBackendView const &view) const noexcept {
    return lookup_or_insert_impl<false>(
            view, bnode_storage_);
}

identifier::NodeID ReferenceNodeStorageBackend::find_id(view::IRIBackendView const &view) const noexcept {
    return lookup_or_insert_impl<false>(
            view, iri_storage_);
}

identifier::NodeID ReferenceNodeStorageBackend::find_id(view::LiteralBackendView const &view) const noexcept {
    return visit(view,
            [this](view::LexicalFormLiteralBackendView const &lexical) {
                return lookup_or_insert_impl<false>(
                        lexical, fallback_literal_storage_);
            },
            [this](view::ValueLiteralBackendView const &any) {
                return visit_specialized(*this, any.datatype, [&any](auto const &storage) {
                    return lookup_or_insert_impl<false>(any, storage);
                });
            });
}

identifier::NodeID ReferenceNodeStorageBackend::find_id(view::VariableBackendView const &view) const noexcept {
    return lookup_or_insert_impl<false>(
            view, variable_storage_);
}

template<typename NodeTypeStorage>
static typename NodeTypeStorage::BackendView find_backend_view(NodeTypeStorage &storage, identifier::NodeID const id) {
    std::shared_lock<std::shared_mutex> shared_lock{storage.mutex};
    return static_cast<typename NodeTypeStorage::BackendView>(*storage.id2data.at(id));
}

view::IRIBackendView ReferenceNodeStorageBackend::find_iri_backend_view(identifier::NodeID const id) const {
    return find_backend_view(iri_storage_, id);
}

view::LiteralBackendView ReferenceNodeStorageBackend::find_literal_backend_view(identifier::NodeID const id) const {
    if (id.literal_type().is_fixed() && this->has_specialized_storage_for(id.literal_type())) {
        return visit_specialized(*this, id.literal_type(), [id](auto const &storage) {
            return find_backend_view(storage, id);
        });
    }

    return find_backend_view(fallback_literal_storage_, id);
}

view::BNodeBackendView ReferenceNodeStorageBackend::find_bnode_backend_view(identifier::NodeID const id) const {
    return find_backend_view(bnode_storage_, id);
}

view::VariableBackendView ReferenceNodeStorageBackend::find_variable_backend_view(identifier::NodeID const id) const {
    return find_backend_view(variable_storage_, id);
}

template<typename NodeTypeStorage>
static bool erase_impl(NodeTypeStorage &storage, identifier::NodeID const id) noexcept {
    std::unique_lock lock{storage.mutex};
    auto it = storage.id2data.find(id);
    if (it == storage.id2data.end()) {
        return false;
    }

    auto const *backend_ptr = it->second.get();

    auto data_it = storage.data2id.find(static_cast<typename NodeTypeStorage::BackendView>(*backend_ptr));
    assert(data_it != storage.data2id.end());

    storage.id2data.erase(it);
    storage.data2id.erase(data_it);

    return true;
}

bool ReferenceNodeStorageBackend::erase_iri(identifier::NodeID const id) {
    return erase_impl(iri_storage_, id);
}

bool ReferenceNodeStorageBackend::erase_literal(identifier::NodeID const id) {
    if (id.literal_type().is_fixed() && this->has_specialized_storage_for(id.literal_type())) {
        return visit_specialized(*this, id.literal_type(), [id](auto &storage) noexcept {
            return erase_impl(storage, id);
        });
    }

    return erase_impl(fallback_literal_storage_, id);
}

bool ReferenceNodeStorageBackend::erase_bnode(identifier::NodeID const id) {
    return erase_impl(bnode_storage_, id);
}

bool ReferenceNodeStorageBackend::erase_variable(identifier::NodeID const id) {
    return erase_impl(variable_storage_, id);
}

}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage