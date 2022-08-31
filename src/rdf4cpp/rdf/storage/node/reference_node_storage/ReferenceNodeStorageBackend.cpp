#include "ReferenceNodeStorageBackend.hpp"

#include <functional>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

ReferenceNodeStorageBackend::ReferenceNodeStorageBackend() : INodeStorageBackend() {
    // some iri's like xsd:string are there by default

    for (const auto &[id, iri] : NodeID::predefined_iris) {
        auto [iter, inserted_successfully] = iri_storage_.data2id.emplace(std::make_unique<IRIBackend>(iri), id);
        assert(inserted_successfully);
        iri_storage_.id2data.emplace(id, iter->first.get());
    }
}

/**
 * Synchronized lookup (and creation) of IDs by a provided view of a Node Backend.
 * @tparam Backend_t the Backend type. One of BNodeBackend, IRIBackend, LiteralBackend or VariableBackend
 * @tparam create_if_not_present enables code for creating non-existing Node Backends
 * @tparam NextIDFromView_func type of a function to generate the next ID which is assigned in case a new Node Backend is created
 * @param view contains the data of the requested Node Backend
 * @param storage the storage where the Node Backend is looked up
 * @param next_id_func function to generate the next ID which is assigned in case a new Node Backend is created
 * @return the NodeID for the looked up Node Backend. Result is null() if there was no matching Node Backend.
 */
template<class Backend_t, bool create_if_not_present, class NextIDFromView_func = void *>
inline identifier::NodeID lookup_or_insert_impl(typename Backend_t::View const &view,
                                                auto &storage,
                                                NextIDFromView_func next_id_func = nullptr) noexcept {
    std::shared_lock<std::shared_mutex> shared_lock{storage.mutex};
    auto found = storage.data2id.find(view);
    if (found == storage.data2id.end()) {
        if constexpr (create_if_not_present) {
            shared_lock.unlock();
            std::unique_lock<std::shared_mutex> unique_lock{storage.mutex};
            // update found (might have changed in the meantime)
            found = storage.data2id.find(view);
            if (found == storage.data2id.end()) {
                identifier::NodeID id = next_id_func(view);
                auto [found2, inserted_successfully] = storage.data2id.emplace(std::make_unique<Backend_t>(view), id);
                assert(inserted_successfully);
                found = std::move(found2);
                storage.id2data.emplace(id, found->first.get());
                return id;
            } else {
                unique_lock.unlock();
                return found->second;
            }
        } else {
            return {};
        }
    } else {
        shared_lock.unlock();
        return found->second;
    }
}

identifier::NodeID ReferenceNodeStorageBackend::find_or_make_id(view::LiteralBackendView const &view) noexcept {
    return lookup_or_insert_impl<LiteralBackend, true>(
            view, literal_storage_,
            [this]([[maybe_unused]] view::LiteralBackendView const &literal_view) {
                return identifier::NodeID{next_literal_id++,
                                          identifier::LiteralType::from_iri_node_id(literal_view.datatype_id.value())};
            });
}

identifier::NodeID ReferenceNodeStorageBackend::find_or_make_id(view::IRIBackendView const &view) noexcept {
    return lookup_or_insert_impl<IRIBackend, true>(
            view, iri_storage_,
            [this]([[maybe_unused]] view::IRIBackendView const &view) {
                return next_iri_id++;
            });
}

identifier::NodeID ReferenceNodeStorageBackend::find_or_make_id(view::BNodeBackendView const &view) noexcept {
    return lookup_or_insert_impl<BNodeBackend, true>(
            view, bnode_storage_,
            [this]([[maybe_unused]] view::BNodeBackendView const &view) {
                return next_bnode_id++;
            });
}
identifier::NodeID ReferenceNodeStorageBackend::find_or_make_id(view::VariableBackendView const &view) noexcept {
    return lookup_or_insert_impl<VariableBackend, true>(
            view, variable_storage_,
            [this]([[maybe_unused]] view::VariableBackendView const &view) {
                return next_variable_id++;
            });
}

identifier::NodeID ReferenceNodeStorageBackend::find_id(const view::BNodeBackendView &view) const noexcept {
    return lookup_or_insert_impl<BNodeBackend, false>(
            view, bnode_storage_);
}
identifier::NodeID ReferenceNodeStorageBackend::find_id(const view::IRIBackendView &view) const noexcept {
    return lookup_or_insert_impl<IRIBackend, false>(
            view, iri_storage_);
}
identifier::NodeID ReferenceNodeStorageBackend::find_id(const view::LiteralBackendView &view) const noexcept {
    return lookup_or_insert_impl<LiteralBackend, false>(
            view, literal_storage_);
}
identifier::NodeID ReferenceNodeStorageBackend::find_id(const view::VariableBackendView &view) const noexcept {
    return lookup_or_insert_impl<VariableBackend, false>(
            view, variable_storage_);
}

template<typename NodeTypeStorage>
typename NodeTypeStorage::BackendView find_backend_view(NodeTypeStorage &storage, identifier::NodeID id) {
    std::shared_lock<std::shared_mutex> shared_lock{storage.mutex};
    return typename NodeTypeStorage::BackendView(*storage.id2data.at(id));
}

view::IRIBackendView ReferenceNodeStorageBackend::find_iri_backend_view(identifier::NodeID id) const {
    return find_backend_view(iri_storage_, id);
}
view::LiteralBackendView ReferenceNodeStorageBackend::find_literal_backend_view(identifier::NodeID id) const {
    return find_backend_view(literal_storage_, id);
}
view::BNodeBackendView ReferenceNodeStorageBackend::find_bnode_backend_view(identifier::NodeID id) const {
    return find_backend_view(bnode_storage_, id);
}
view::VariableBackendView ReferenceNodeStorageBackend::find_variable_backend_view(identifier::NodeID id) const {
    return find_backend_view(variable_storage_, id);
}
bool ReferenceNodeStorageBackend::erase_iri([[maybe_unused]] identifier::NodeID id) const {
    throw std::runtime_error("Deleting nodes is not implemented in ReferenceNodeStorageBackend.");
}
bool ReferenceNodeStorageBackend::erase_literal([[maybe_unused]] identifier::NodeID id) const {
    throw std::runtime_error("Deleting nodes is not implemented in ReferenceNodeStorageBackend.");
}
bool ReferenceNodeStorageBackend::erase_bnode([[maybe_unused]] identifier::NodeID id) const {
    throw std::runtime_error("Deleting nodes is not implemented in ReferenceNodeStorageBackend.");
}
bool ReferenceNodeStorageBackend::erase_variable([[maybe_unused]] identifier::NodeID id) const {
    throw std::runtime_error("Deleting nodes is not implemented in ReferenceNodeStorageBackend.");
}
}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage