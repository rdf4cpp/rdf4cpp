#include "SyncReferenceNodeStorage.hpp"

#include <dice/template-library/tuple_algorithm.hpp>
#include <rdf4cpp/storage/reference_node_storage/detail/SpecializationDetail.hpp>

namespace rdf4cpp::storage::reference_node_storage {

SyncReferenceNodeStorage::SyncReferenceNodeStorage() noexcept {
    iri_storage_.mapping.reserve_until(identifier::NodeID::min_iri_id);
    bnode_storage_.mapping.reserve_until(identifier::NodeID::min_bnode_id);
    variable_storage_.mapping.reserve_until(identifier::NodeID::min_variable_id);
    fallback_literal_storage_.mapping.reserve_until(identifier::NodeID::min_literal_id);

    dice::template_library::tuple_for_each(specialized_literal_storage_, [](auto &storage) {
        storage.mapping.reserve_until(identifier::NodeID::min_literal_id);
    });

    // some iri's like xsd:string are there by default
    for (const auto &[iri, literal_type] : datatypes::registry::reserved_datatype_ids) {
        auto const id = literal_type.to_underlying();
        iri_storage_.mapping.insert_assume_not_present_at(view::IRIBackendView{.identifier = iri}, identifier::NodeID{id});
    }
}

template<typename Backend>
static size_t storage_size(SyncNodeTypeStorage<Backend> const &storage) noexcept {
    std::shared_lock<std::shared_mutex> l{storage.mutex};
    return storage.mapping.size();
}

size_t SyncReferenceNodeStorage::size() const noexcept {
    return storage_size(iri_storage_) +
           storage_size(bnode_storage_) +
           storage_size(variable_storage_) +
           storage_size(fallback_literal_storage_) +
           dice::template_library::tuple_fold(specialized_literal_storage_, 0, [](auto acc, auto const &storage) noexcept {
               return acc + storage_size(storage);
           });
}

template<typename Backend>
static void storage_shrink_to_fit(SyncNodeTypeStorage<Backend> &storage) {
    std::unique_lock<std::shared_mutex> l{storage.mutex};
    storage.mapping.shrink_to_fit();
}

void SyncReferenceNodeStorage::shrink_to_fit() {
    storage_shrink_to_fit(iri_storage_);
    storage_shrink_to_fit(bnode_storage_);
    storage_shrink_to_fit(variable_storage_);
    storage_shrink_to_fit(fallback_literal_storage_);

    dice::template_library::tuple_for_each(specialized_literal_storage_, [](auto &storage) {
        storage_shrink_to_fit(storage);
    });
}

bool SyncReferenceNodeStorage::has_specialized_storage_for(identifier::LiteralType const datatype) noexcept {
    static constexpr auto specialization_lut = specialization_detail::make_storage_specialization_lut<decltype(specialized_literal_storage_)>();
    return specialization_lut[datatype.to_underlying()];
}

/**
 * Synchronized lookup (and creation) of IDs by a provided view of a Node Backend.
 * @tparam create_if_not_present enables code for creating non-existing Node Backends
 * @param view contains the data of the requested Node Backend
 * @param storage the storage where the Node Backend is looked up
 * @return the NodeID for the looked up Node Backend. Result is the null-id if there was no matching Node Backend.
 */
template<bool create_if_not_present, typename Storage>
static identifier::NodeID lookup_or_insert_impl(typename Storage::backend_view_type const &view,
                                                Storage &storage) noexcept(!create_if_not_present) {

    {
        std::shared_lock lock{storage.mutex};
        if (auto const id = storage.mapping.lookup_id(view); id != typename Storage::backend_id_type{}) {
            return Storage::to_node_id(id, view);
        }
    }

    if constexpr (!create_if_not_present) {
        return identifier::NodeID{};
    } else {
        std::unique_lock lock{storage.mutex};

        // check again, might have changed between unlocking of shared_lock and locking of unique_lock
        if (auto const id = storage.mapping.lookup_id(view); id != typename Storage::backend_id_type{}) {
            return Storage::to_node_id(id, view);
        }

        auto const id = storage.mapping.insert_assume_not_present(view);
        return Storage::to_node_id(id, view);
    }
}

identifier::NodeID SyncReferenceNodeStorage::find_or_make_id(view::LiteralBackendView const &view) {
    return view.visit(
            [this](view::LexicalFormLiteralBackendView const &lexical) {
                assert(!has_specialized_storage_for(identifier::iri_node_id_to_literal_type(lexical.datatype_id)));
                return lookup_or_insert_impl<true>(lexical, this->fallback_literal_storage_);
            },
            [this](view::ValueLiteralBackendView const &any) {
                assert(has_specialized_storage_for(any.datatype));
                return specialization_detail::visit_specialized(this->specialized_literal_storage_, any.datatype, [&any](auto &storage) {
                    return lookup_or_insert_impl<true>(any, storage);
                });
            });
}

identifier::NodeID SyncReferenceNodeStorage::find_or_make_id(view::IRIBackendView const &view) {
    return lookup_or_insert_impl<true>(view, iri_storage_);
}

identifier::NodeID SyncReferenceNodeStorage::find_or_make_id(view::BNodeBackendView const &view) {
    return lookup_or_insert_impl<true>(view, bnode_storage_);
}

identifier::NodeID SyncReferenceNodeStorage::find_or_make_id(view::VariableBackendView const &view) {
    return lookup_or_insert_impl<true>(view, variable_storage_);
}

identifier::NodeID SyncReferenceNodeStorage::find_id(view::BNodeBackendView const &view) const noexcept {
    return lookup_or_insert_impl<false>(view, bnode_storage_);
}

identifier::NodeID SyncReferenceNodeStorage::find_id(view::IRIBackendView const &view) const noexcept {
    return lookup_or_insert_impl<false>(view, iri_storage_);
}

identifier::NodeID SyncReferenceNodeStorage::find_id(view::LiteralBackendView const &view) const noexcept {
    return view.visit(
            [this](view::LexicalFormLiteralBackendView const &lexical) noexcept {
                assert(!has_specialized_storage_for(identifier::iri_node_id_to_literal_type(lexical.datatype_id)));
                return lookup_or_insert_impl<false>(lexical, this->fallback_literal_storage_);
            },
            [this](view::ValueLiteralBackendView const &any) noexcept {
                return specialization_detail::visit_specialized(this->specialized_literal_storage_, any.datatype, [&any](auto const &storage) noexcept {
                    assert(has_specialized_storage_for(any.datatype));
                    return lookup_or_insert_impl<false>(any, storage);
                });
            });
}

identifier::NodeID SyncReferenceNodeStorage::find_id(view::VariableBackendView const &view) const noexcept {
    return lookup_or_insert_impl<false>(view, variable_storage_);
}

template<typename Storage>
static typename Storage::backend_view_type find_backend_view(Storage &storage, identifier::NodeID const id) noexcept {
    std::shared_lock<std::shared_mutex> shared_lock{storage.mutex};

    if (auto view = storage.mapping.lookup_value(Storage::to_backend_id(id)); view.has_value()) {
        return *view;
    } else {
        assert(false); // assert in debug build; not critical error but should not happen
        return Storage::get_default_view();
    }
}

view::IRIBackendView SyncReferenceNodeStorage::find_iri_backend(identifier::NodeID const id) const noexcept {
    return find_backend_view(iri_storage_, id);
}

view::LiteralBackendView SyncReferenceNodeStorage::find_literal_backend(identifier::NodeID const id) const noexcept {
    if (id.literal_type().is_fixed() && has_specialized_storage_for(id.literal_type())) {
        return specialization_detail::visit_specialized(specialized_literal_storage_, id.literal_type(), [id](auto const &storage) noexcept {
            return find_backend_view(storage, id);
        });
    }

    return find_backend_view(fallback_literal_storage_, id);
}

view::BNodeBackendView SyncReferenceNodeStorage::find_bnode_backend(identifier::NodeID const id) const noexcept {
    return find_backend_view(bnode_storage_, id);
}

view::VariableBackendView SyncReferenceNodeStorage::find_variable_backend(identifier::NodeID const id) const noexcept {
    return find_backend_view(variable_storage_, id);
}

template<typename Storage>
static bool erase_impl(Storage &storage, identifier::NodeID const id) {
    std::unique_lock lock{storage.mutex};

    auto const backend_id = Storage::to_backend_id(id);
    if (!storage.mapping.lookup_value(backend_id).has_value()) {
        return false;
    }

    storage.mapping.erase_assume_present(backend_id);
    return true;
}

bool SyncReferenceNodeStorage::erase_iri(identifier::NodeID const id) {
    return erase_impl(iri_storage_, id);
}

bool SyncReferenceNodeStorage::erase_literal(identifier::NodeID const id) {
    if (id.literal_type().is_fixed() && has_specialized_storage_for(id.literal_type())) {
        return specialization_detail::visit_specialized(specialized_literal_storage_, id.literal_type(), [id](auto &storage) noexcept {
            return erase_impl(storage, id);
        });
    }

    return erase_impl(fallback_literal_storage_, id);
}

bool SyncReferenceNodeStorage::erase_bnode(identifier::NodeID const id) {
    return erase_impl(bnode_storage_, id);
}

bool SyncReferenceNodeStorage::erase_variable(identifier::NodeID const id) {
    return erase_impl(variable_storage_, id);
}

}  // namespace rdf4cpp::storage::reference_node_storage
