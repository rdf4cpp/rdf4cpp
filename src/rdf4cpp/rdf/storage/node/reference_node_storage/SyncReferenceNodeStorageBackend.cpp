#include <rdf4cpp/rdf/storage/node/reference_node_storage/detail/SpecializationDetail.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/SyncReferenceNodeStorageBackend.hpp>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

SyncReferenceNodeStorageBackend::SyncReferenceNodeStorageBackend() noexcept {
    // set correct initial value for atomics
    for (auto &id : next_specialized_literal_ids_) {
        id = NodeID::min_literal_id.value;
    }

    // some iri's like xsd:string are there by default
    for (const auto &[iri, literal_type] : datatypes::registry::reserved_datatype_ids) {
        auto const id = literal_type.to_underlying();

        auto const [it, inserted] = iri_storage_.id2data.emplace(id, std::make_unique<IRIBackend>(view::IRIBackendView{.identifier = iri}));
        assert(inserted);
        iri_storage_.data2id.emplace(it->second.get(), id);
    }
}

template<typename Backend>
static size_t storage_size(SyncNodeTypeStorage<Backend> const &storage) noexcept {
    std::shared_lock<std::shared_mutex> l{storage.mutex};
    return storage.id2data.size();
}

size_t SyncReferenceNodeStorageBackend::size() const noexcept {
    return storage_size(iri_storage_) +
           storage_size(bnode_storage_) +
           storage_size(variable_storage_) +
           storage_size(fallback_literal_storage_) +
           specialization_detail::tuple_fold(specialized_literal_storage_, 0, [](auto acc, auto const &storage) noexcept {
               return acc + storage_size(storage);
           });
}

bool SyncReferenceNodeStorageBackend::has_specialized_storage_for(identifier::LiteralType const datatype) const noexcept {
    static constexpr auto specialization_lut = specialization_detail::make_storage_specialization_lut<decltype(specialized_literal_storage_)>();
    return specialization_lut[datatype.to_underlying()];
}

/**
 * Synchronized lookup (and creation) of IDs by a provided view of a Node Backend.
 * @tparam create_if_not_present enables code for creating non-existing Node Backends
 * @param view contains the data of the requested Node Backend
 * @param storage the storage where the Node Backend is looked up
 * @param next_id_func function to generate the next ID which is assigned in case a new Node Backend is created
 * @return the NodeID for the looked up Node Backend. Result is the null-id if there was no matching Node Backend.
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

identifier::NodeID SyncReferenceNodeStorageBackend::find_or_make_id(view::LiteralBackendView const &view) noexcept {
    return view.visit(
            [this](view::LexicalFormLiteralBackendView const &lexical) noexcept {
                auto const datatype = identifier::iri_node_id_to_literal_type(lexical.datatype_id);
                assert(!this->has_specialized_storage_for(datatype));

                auto const next_id_func = [this, datatype]() noexcept {
                    auto const id = next_fallback_literal_id_.fetch_add(1, std::memory_order_relaxed);
                    if (id >= (1ul << LiteralID::width)) [[unlikely]] {
                        std::abort();
                    }

                    return identifier::NodeID{identifier::LiteralID{id}, datatype};
                };

                return lookup_or_insert_impl<true>(lexical, fallback_literal_storage_, next_id_func);
            },
            [this](view::ValueLiteralBackendView const &any) noexcept {
                assert(this->has_specialized_storage_for(any.datatype));

                auto const next_id_func = [this, datatype = any.datatype]() noexcept {
                    return specialization_detail::visit_specialized(next_specialized_literal_ids_, datatype, [datatype](auto &lit_id) {
                        auto const id = lit_id.fetch_add(1, std::memory_order_relaxed);
                        if (id >= (1ul << LiteralID::width)) [[unlikely]] {
                            std::abort();
                        }

                        return identifier::NodeID{LiteralID{id}, datatype};
                    });
                };

                return specialization_detail::visit_specialized(specialized_literal_storage_, any.datatype, [&](auto &storage) noexcept {
                    return lookup_or_insert_impl<true>(any, storage, next_id_func);
                });
            });
}

identifier::NodeID SyncReferenceNodeStorageBackend::find_or_make_id(view::IRIBackendView const &view) noexcept {
    return lookup_or_insert_impl<true>(view, iri_storage_, [this]() noexcept {
        auto const id = next_iri_id_.fetch_add(1, std::memory_order_relaxed);
        if (id >= (1ul << NodeID::width)) [[unlikely]] {
            std::abort();
        }

        return identifier::NodeID{id};
    });
}

identifier::NodeID SyncReferenceNodeStorageBackend::find_or_make_id(view::BNodeBackendView const &view) noexcept {
    return lookup_or_insert_impl<true>(view, bnode_storage_, [this]() noexcept {
        auto const id = next_bnode_id_.fetch_add(1, std::memory_order_relaxed);
        if (id >= (1ul << NodeID::width)) [[unlikely]] {
            std::abort();
        }

        return identifier::NodeID{id};
    });
}

identifier::NodeID SyncReferenceNodeStorageBackend::find_or_make_id(view::VariableBackendView const &view) noexcept {
    return lookup_or_insert_impl<true>(view, variable_storage_, [this]() noexcept {
        auto const id = next_variable_id_.fetch_add(1, std::memory_order_relaxed);
        if (id >= (1ul << NodeID::width)) [[unlikely]] {
            std::abort();
        }

        return identifier::NodeID{id};
    });
}

identifier::NodeID SyncReferenceNodeStorageBackend::find_id(view::BNodeBackendView const &view) const noexcept {
    return lookup_or_insert_impl<false>(view, bnode_storage_);
}

identifier::NodeID SyncReferenceNodeStorageBackend::find_id(view::IRIBackendView const &view) const noexcept {
    return lookup_or_insert_impl<false>(view, iri_storage_);
}

identifier::NodeID SyncReferenceNodeStorageBackend::find_id(view::LiteralBackendView const &view) const noexcept {
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

identifier::NodeID SyncReferenceNodeStorageBackend::find_id(view::VariableBackendView const &view) const noexcept {
    return lookup_or_insert_impl<false>(view, variable_storage_);
}

template<typename NodeTypeStorage>
static typename NodeTypeStorage::BackendView find_backend_view(NodeTypeStorage &storage, identifier::NodeID const id) {
    std::shared_lock<std::shared_mutex> shared_lock{storage.mutex};
    return static_cast<typename NodeTypeStorage::BackendView>(*storage.id2data.at(id));
}

view::IRIBackendView SyncReferenceNodeStorageBackend::find_iri_backend_view(identifier::NodeID const id) const {
    return find_backend_view(iri_storage_, id);
}

view::LiteralBackendView SyncReferenceNodeStorageBackend::find_literal_backend_view(identifier::NodeID const id) const {
    if (id.literal_type().is_fixed() && this->has_specialized_storage_for(id.literal_type())) {
        return specialization_detail::visit_specialized(specialized_literal_storage_, id.literal_type(), [id](auto const &storage) {
            return find_backend_view(storage, id);
        });
    }

    return find_backend_view(fallback_literal_storage_, id);
}

view::BNodeBackendView SyncReferenceNodeStorageBackend::find_bnode_backend_view(identifier::NodeID const id) const {
    return find_backend_view(bnode_storage_, id);
}

view::VariableBackendView SyncReferenceNodeStorageBackend::find_variable_backend_view(identifier::NodeID const id) const {
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

bool SyncReferenceNodeStorageBackend::erase_iri(identifier::NodeID const id) noexcept {
    return erase_impl(iri_storage_, id);
}

bool SyncReferenceNodeStorageBackend::erase_literal(identifier::NodeID const id) noexcept {
    if (id.literal_type().is_fixed() && this->has_specialized_storage_for(id.literal_type())) {
        return specialization_detail::visit_specialized(specialized_literal_storage_, id.literal_type(), [id](auto &storage) noexcept {
            return erase_impl(storage, id);
        });
    }

    return erase_impl(fallback_literal_storage_, id);
}

bool SyncReferenceNodeStorageBackend::erase_bnode(identifier::NodeID const id) noexcept {
    return erase_impl(bnode_storage_, id);
}

bool SyncReferenceNodeStorageBackend::erase_variable(identifier::NodeID const id) noexcept {
    return erase_impl(variable_storage_, id);
}

}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage
