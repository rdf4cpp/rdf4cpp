#include "UnsyncReferenceNodeStorageBackend.hpp"

#include <dice/template-library/tuple_algorithm.hpp>
#include <rdf4cpp/storage/reference_node_storage/detail/SpecializationDetail.hpp>

namespace rdf4cpp::storage::reference_node_storage {

UnsyncReferenceNodeStorageBackend::UnsyncReferenceNodeStorageBackend() noexcept {
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

size_t UnsyncReferenceNodeStorageBackend::size() const noexcept {
    return iri_storage_.mapping.size() +
           bnode_storage_.mapping.size() +
           variable_storage_.mapping.size() +
           fallback_literal_storage_.mapping.size() +
           dice::template_library::tuple_fold(specialized_literal_storage_, 0, [](auto acc, auto const &storage) noexcept {
               return acc + storage.mapping.size();
           });
}

void UnsyncReferenceNodeStorageBackend::shrink_to_fit() {
    iri_storage_.mapping.shrink_to_fit();
    bnode_storage_.mapping.shrink_to_fit();
    variable_storage_.mapping.shrink_to_fit();
    fallback_literal_storage_.mapping.shrink_to_fit();

    dice::template_library::tuple_for_each(specialized_literal_storage_, [](auto &storage) {
        storage.mapping.shrink_to_fit();
    });
}

bool UnsyncReferenceNodeStorageBackend::has_specialized_storage_for(identifier::LiteralType const datatype) noexcept {
    static constexpr auto specialization_lut = specialization_detail::make_storage_specialization_lut<decltype(specialized_literal_storage_)>();
    return specialization_lut[datatype.to_underlying()];
}

/**
 * Unsynchronized lookup (and creation) of IDs by a provided view of a Node Backend.
 * @tparam create_if_not_present enables code for creating non-existing Node Backends
 * @param view contains the data of the requested Node Backend
 * @param storage the storage where the Node Backend is looked up
 * @return the NodeID for the looked up Node Backend. Result is the null-id if there was no matching Node Backend.
 */
template<bool create_if_not_present, typename Storage>
static identifier::NodeID lookup_or_insert_impl(typename Storage::backend_view_type const &view,
                                                Storage &storage) noexcept {

    if (auto const id = storage.mapping.lookup_id(view); id != typename Storage::backend_id_type{}) {
        return Storage::to_node_id(id, view);
    }

    if constexpr (!create_if_not_present) {
        return identifier::NodeID{};
    } else {
        auto const id = storage.mapping.insert_assume_not_present(view);
        return Storage::to_node_id(id, view);
    }
}

identifier::NodeID UnsyncReferenceNodeStorageBackend::find_or_make_id(view::LiteralBackendView const &view) noexcept {
    return view.visit(
            [this](view::LexicalFormLiteralBackendView const &lexical) noexcept {
                auto const datatype = identifier::iri_node_id_to_literal_type(lexical.datatype_id);
                assert(!this->has_specialized_storage_for(datatype));

                return lookup_or_insert_impl<true>(lexical, fallback_literal_storage_);
            },
            [this](view::ValueLiteralBackendView const &any) noexcept {
                assert(this->has_specialized_storage_for(any.datatype));

                return specialization_detail::visit_specialized(specialized_literal_storage_, any.datatype, [&](auto &storage) noexcept {
                    return lookup_or_insert_impl<true>(any, storage);
                });
            });
}

identifier::NodeID UnsyncReferenceNodeStorageBackend::find_or_make_id(view::IRIBackendView const &view) noexcept {
    return lookup_or_insert_impl<true>(view, iri_storage_);
}

identifier::NodeID UnsyncReferenceNodeStorageBackend::find_or_make_id(view::BNodeBackendView const &view) noexcept {
    return lookup_or_insert_impl<true>(view, bnode_storage_);
}

identifier::NodeID UnsyncReferenceNodeStorageBackend::find_or_make_id(view::VariableBackendView const &view) noexcept {
    return lookup_or_insert_impl<true>(view, variable_storage_);
}

identifier::NodeID UnsyncReferenceNodeStorageBackend::find_id(view::BNodeBackendView const &view) const noexcept {
    return lookup_or_insert_impl<false>(view, bnode_storage_);
}

identifier::NodeID UnsyncReferenceNodeStorageBackend::find_id(view::IRIBackendView const &view) const noexcept {
    return lookup_or_insert_impl<false>(view, iri_storage_);
}

identifier::NodeID UnsyncReferenceNodeStorageBackend::find_id(view::LiteralBackendView const &view) const noexcept {
    return view.visit(
            [this](view::LexicalFormLiteralBackendView const &lexical) {
                assert(!this->has_specialized_storage_for(identifier::iri_node_id_to_literal_type(lexical.datatype_id)));
                return lookup_or_insert_impl<false>(lexical, fallback_literal_storage_);
            },
            [this](view::ValueLiteralBackendView const &any) {
                return specialization_detail::visit_specialized(specialized_literal_storage_, any.datatype, [this, &any](auto const &storage) {
                    assert(this->has_specialized_storage_for(any.datatype));
                    (void) this;

                    return lookup_or_insert_impl<false>(any, storage);
                });
            });
}

identifier::NodeID UnsyncReferenceNodeStorageBackend::find_id(view::VariableBackendView const &view) const noexcept {
    return lookup_or_insert_impl<false>(view, variable_storage_);
}

template<typename NodeTypeStorage>
static typename NodeTypeStorage::backend_view_type find_backend_view(NodeTypeStorage &storage, identifier::NodeID const id) {
    if (auto view = storage.mapping.lookup_value(NodeTypeStorage::to_backend_id(id)); view.has_value()) {
        return *view;
    } else {
        throw std::out_of_range{"Did not find node for given id"};
    }
}

view::IRIBackendView UnsyncReferenceNodeStorageBackend::find_iri_backend_view(identifier::NodeID const id) const {
    return find_backend_view(iri_storage_, id);
}

view::LiteralBackendView UnsyncReferenceNodeStorageBackend::find_literal_backend_view(identifier::NodeID const id) const {
    if (id.literal_type().is_fixed() && this->has_specialized_storage_for(id.literal_type())) {
        return specialization_detail::visit_specialized(specialized_literal_storage_, id.literal_type(), [id](auto const &storage) {
            return find_backend_view(storage, id);
        });
    }

    return find_backend_view(fallback_literal_storage_, id);
}

view::BNodeBackendView UnsyncReferenceNodeStorageBackend::find_bnode_backend_view(identifier::NodeID const id) const {
    return find_backend_view(bnode_storage_, id);
}

view::VariableBackendView UnsyncReferenceNodeStorageBackend::find_variable_backend_view(identifier::NodeID const id) const {
    return find_backend_view(variable_storage_, id);
}

template<typename Storage>
static bool erase_impl(Storage &storage, identifier::NodeID const id) noexcept {
    auto const backend_id = Storage::to_backend_id(id);
    if (!storage.mapping.lookup_value(backend_id).has_value()) {
        return false;
    }

    storage.mapping.erase_assume_present(backend_id);
    return true;
}

bool UnsyncReferenceNodeStorageBackend::erase_iri(identifier::NodeID const id) noexcept {
    return erase_impl(iri_storage_, id);
}

bool UnsyncReferenceNodeStorageBackend::erase_literal(identifier::NodeID const id) noexcept {
    if (id.literal_type().is_fixed() && this->has_specialized_storage_for(id.literal_type())) {
        return specialization_detail::visit_specialized(specialized_literal_storage_, id.literal_type(), [id](auto &storage) noexcept {
            return erase_impl(storage, id);
        });
    }

    return erase_impl(fallback_literal_storage_, id);
}

bool UnsyncReferenceNodeStorageBackend::erase_bnode(identifier::NodeID const id) noexcept {
    return erase_impl(bnode_storage_, id);
}

bool UnsyncReferenceNodeStorageBackend::erase_variable(identifier::NodeID const id) noexcept {
    return erase_impl(variable_storage_, id);
}

void UnsyncReferenceNodeStorageBackend::clear() noexcept {
    iri_storage_.mapping.clear();
    bnode_storage_.mapping.clear();
    variable_storage_.mapping.clear();
    fallback_literal_storage_.mapping.clear();

    dice::template_library::tuple_for_each(specialized_literal_storage_, [](auto &storage) {
        storage.mapping.clear();
    });
}

}  // namespace rdf4cpp::storage::reference_node_storage