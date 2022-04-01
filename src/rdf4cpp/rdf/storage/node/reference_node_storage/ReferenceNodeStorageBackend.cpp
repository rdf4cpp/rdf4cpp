#include "ReferenceNodeStorageBackend.hpp"

#include <functional>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

ReferenceNodeStorageBackend::ReferenceNodeStorageBackend() : INodeStorageBackend() {
    // TODO: that should be done by (Abstract)NodeContextBackend
    // some iri's like xsd:string are there by default
    for (const auto &[id, iri] : NodeID::predefined_iris) {
        auto [iter, inserted_successfully] = iri_storage_reverse.emplace(std::make_unique<IRIBackend>(iri), id);
        assert(inserted_successfully);
        iri_storage.insert({id, iter->first.get()});
    }
}

template<class Backend_t, bool create_if_not_present, class View_t, class Storage_t, class ReverseStorage_t, class NextIDFromView_func = void *>
inline identifier::NodeID lookup_or_insert_impl(View_t view, std::shared_mutex &mutex, Storage_t &storage,
                                                ReverseStorage_t &reverse_storage,
                                                NextIDFromView_func next_id_func = nullptr) noexcept {
    std::shared_lock<std::shared_mutex> shared_lock{mutex};
    auto found = reverse_storage.find(view);
    if (found == reverse_storage.end()) {
        if constexpr (create_if_not_present) {
            shared_lock.unlock();
            std::unique_lock<std::shared_mutex> unique_lock{mutex};
            // update found (might have changed in the meantime)
            found = reverse_storage.find(view);
            if (found == reverse_storage.end()) {
                identifier::NodeID id = next_id_func(view);
                auto [found2, inserted_successfully] = reverse_storage.emplace(std::make_unique<typename ReverseStorage_t::key_type::element_type>(view), id);
                assert(inserted_successfully);
                found = std::move(found2);
                storage.insert({id, found->first.get()});
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
            view, literal_mutex_, literal_storage, literal_storage_reverse,
            [this]([[maybe_unused]] view::LiteralBackendView const &literal_view) {
                // TODO: actually use LiteralType (therefore, we will need literal_view)
                return identifier::NodeID{next_literal_id++, identifier::LiteralType::OTHER};
            });
}

identifier::NodeID ReferenceNodeStorageBackend::find_or_make_id(view::IRIBackendView const &view) noexcept {
    return lookup_or_insert_impl<IRIBackend, true>(
            view, iri_mutex_, iri_storage, iri_storage_reverse,
            [this]([[maybe_unused]] view::IRIBackendView const &view) {
                return next_iri_id++;
            });
}

identifier::NodeID ReferenceNodeStorageBackend::find_or_make_id(view::BNodeBackendView const &view) noexcept {
    return lookup_or_insert_impl<BNodeBackend, true>(
            view, bnode_mutex_, bnode_storage, bnode_storage_reverse,
            [this]([[maybe_unused]] view::BNodeBackendView const &view) {
                return next_bnode_id++;
            });
}
identifier::NodeID ReferenceNodeStorageBackend::find_or_make_id(view::VariableBackendView const &view) noexcept {
    return lookup_or_insert_impl<VariableBackend, true>(
            view, variable_mutex_, variable_storage, variable_storage_reverse,
            [this]([[maybe_unused]] view::VariableBackendView const &view) {
                return next_variable_id++;
            });
}

identifier::NodeID ReferenceNodeStorageBackend::find_id(const view::BNodeBackendView &view) const noexcept {
    return lookup_or_insert_impl<BNodeBackend, false>(
            view, bnode_mutex_, bnode_storage, bnode_storage_reverse);
}
identifier::NodeID ReferenceNodeStorageBackend::find_id(const view::IRIBackendView &view) const noexcept {
    return lookup_or_insert_impl<IRIBackend, false>(
            view, iri_mutex_, iri_storage, iri_storage_reverse);
}
identifier::NodeID ReferenceNodeStorageBackend::find_id(const view::LiteralBackendView &view) const noexcept {
    return lookup_or_insert_impl<LiteralBackend, false>(
            view, literal_mutex_, literal_storage, literal_storage_reverse);
}
identifier::NodeID ReferenceNodeStorageBackend::find_id(const view::VariableBackendView &view) const noexcept {
    return lookup_or_insert_impl<VariableBackend, false>(
            view, variable_mutex_, variable_storage, variable_storage_reverse);
}

view::IRIBackendView ReferenceNodeStorageBackend::find_iri_backend_view(identifier::NodeID id) const {
    std::shared_lock<std::shared_mutex> shared_lock{iri_mutex_};
    return view::IRIBackendView(*iri_storage.at(id));
}
view::LiteralBackendView ReferenceNodeStorageBackend::find_literal_backend_view(identifier::NodeID id) const {
    std::shared_lock<std::shared_mutex> shared_lock{literal_mutex_};
    return view::LiteralBackendView(*literal_storage.at(id));
}
view::BNodeBackendView ReferenceNodeStorageBackend::find_bnode_backend_view(identifier::NodeID id) const {
    std::shared_lock<std::shared_mutex> shared_lock{bnode_mutex_};
    return view::BNodeBackendView(*bnode_storage.at(id));
}
view::VariableBackendView ReferenceNodeStorageBackend::find_variable_backend_view(identifier::NodeID id) const {
    std::shared_lock<std::shared_mutex> shared_lock{variable_mutex_};
    return view::VariableBackendView(*variable_storage.at(id));
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