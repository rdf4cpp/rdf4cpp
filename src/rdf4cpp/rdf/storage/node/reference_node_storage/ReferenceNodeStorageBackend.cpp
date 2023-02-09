#include "ReferenceNodeStorageBackend.hpp"

#include <rdf4cpp/rdf/storage/util/Overloaded.hpp>
#include <rdf4cpp/rdf/datatypes/registry/util/StaticFlatMap.hpp>
#include <functional>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

static consteval std::array<bool, 1 << identifier::LiteralType::width> make_storage_specialization_lut() noexcept {
    using Entry = std::pair<identifier::LiteralType, bool>;

    datatypes::registry::util::StaticFlatMap<identifier::LiteralType, bool, 1 << identifier::LiteralType::width> base{
        Entry{datatypes::xsd::Long::fixed_id, true}};

    std::array<bool, 1 << identifier::LiteralType::width> ret{};
    for (auto const &[type, include] : base) {
        ret[type.to_underlying()] = include;
    }

    return ret;
}

static constexpr std::array<bool, 1 << identifier::LiteralType::width> storage_specialization_lut = make_storage_specialization_lut();

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
    return this->iri_storage_.id2data.size() + this->bnode_storage_.id2data.size() + this->fallback_literal_storage_.id2data.size() + this->variable_storage_.id2data.size();
}

bool ReferenceNodeStorageBackend::has_specialized_storage_for(identifier::LiteralType datatype) const noexcept {
    return storage_specialization_lut[datatype.to_underlying()];
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
template<bool create_if_not_present, class Storage, class NextIDFromView_func = void *>
static identifier::NodeID lookup_or_insert_impl(typename Storage::BackendView const &view,
                                                Storage &storage,
                                                NextIDFromView_func next_id_func = nullptr) noexcept {

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
    return std::visit(util::Overloaded{
                              [&](view::LexicalFormBackendView const &lexical) {
                                  return lookup_or_insert_impl<true>(
                                          lexical, fallback_literal_storage_,
                                          [&]() {
                                              return identifier::NodeID{next_literal_id++, identifier::iri_node_id_to_literal_type(lexical.datatype_id)};
                                          });
                              },
                              [&](view::AnyBackendView const &any) {
                                  switch (any.datatype.to_underlying()) {
                                      case datatypes::xsd::Int::fixed_id.to_underlying():
                                          return lookup_or_insert_impl<true>(
                                                  any, int_literal_storage_,
                                                  [&]() {
                                                      return identifier::NodeID{next_literal_id++, any.datatype};
                                                  });
                                      default:
                                          assert(false);
                                  }
                              }
                      },
                      view.literal);
}

identifier::NodeID ReferenceNodeStorageBackend::find_or_make_id(view::IRIBackendView const &view) noexcept {
    return lookup_or_insert_impl<true>(
            view, iri_storage_,
            [this]() {
                return next_iri_id++;
            });
}

identifier::NodeID ReferenceNodeStorageBackend::find_or_make_id(view::BNodeBackendView const &view) noexcept {
    return lookup_or_insert_impl<true>(
            view, bnode_storage_,
            [this]() {
                return next_bnode_id++;
            });
}
identifier::NodeID ReferenceNodeStorageBackend::find_or_make_id(view::VariableBackendView const &view) noexcept {
    return lookup_or_insert_impl<true>(
            view, variable_storage_,
            [this]() {
                return next_variable_id++;
            });
}

identifier::NodeID ReferenceNodeStorageBackend::find_id(const view::BNodeBackendView &view) const noexcept {
    return lookup_or_insert_impl<false>(
            view, bnode_storage_);
}
identifier::NodeID ReferenceNodeStorageBackend::find_id(const view::IRIBackendView &view) const noexcept {
    return lookup_or_insert_impl<false>(
            view, iri_storage_);
}
identifier::NodeID ReferenceNodeStorageBackend::find_id(const view::LiteralBackendView &view) const noexcept {
    return std::visit(util::Overloaded{
                              [&](view::LexicalFormBackendView const &lexical) {
                                  return lookup_or_insert_impl<false>(
                                          lexical, fallback_literal_storage_);
                              },
                              [&](view::AnyBackendView const &any) {
                                  switch (any.datatype.to_underlying()) {
                                      case datatypes::xsd::Long::fixed_id.to_underlying():
                                          return lookup_or_insert_impl<false>(
                                                  any, int_literal_storage_);
                                      default:
                                          assert(false);
                                  }
                              }
                      },
                      view.literal);
}
identifier::NodeID ReferenceNodeStorageBackend::find_id(const view::VariableBackendView &view) const noexcept {
    return lookup_or_insert_impl<false>(
            view, variable_storage_);
}

template<typename NodeTypeStorage>
static typename NodeTypeStorage::BackendView find_backend_view(NodeTypeStorage &storage, identifier::NodeID id) {
    std::shared_lock<std::shared_mutex> shared_lock{storage.mutex};
    return static_cast<typename NodeTypeStorage::BackendView>(*storage.id2data.at(id));
}

view::IRIBackendView ReferenceNodeStorageBackend::find_iri_backend_view(identifier::NodeID id) const {
    return find_backend_view(iri_storage_, id);
}
view::LiteralBackendView ReferenceNodeStorageBackend::find_literal_backend_view(identifier::NodeID id) const {
    switch (id.literal_type().to_underlying()) {
        case datatypes::xsd::Int::fixed_id.to_underlying():
            return view::LiteralBackendView{ .literal = find_backend_view(int_literal_storage_, id) };
        default:
            return view::LiteralBackendView{ .literal = find_backend_view(fallback_literal_storage_, id) };
    }
}
view::BNodeBackendView ReferenceNodeStorageBackend::find_bnode_backend_view(identifier::NodeID id) const {
    return find_backend_view(bnode_storage_, id);
}
view::VariableBackendView ReferenceNodeStorageBackend::find_variable_backend_view(identifier::NodeID id) const {
    return find_backend_view(variable_storage_, id);
}

template<typename NodeTypeStorage>
static bool erase_impl(NodeTypeStorage &storage, identifier::NodeID id) noexcept {
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

bool ReferenceNodeStorageBackend::erase_iri([[maybe_unused]] identifier::NodeID id) {
    return erase_impl(iri_storage_, id);
}
bool ReferenceNodeStorageBackend::erase_literal([[maybe_unused]] identifier::NodeID id) {
    switch (id.literal_type().to_underlying()) {
        case datatypes::xsd::Int::fixed_id.to_underlying():
            return erase_impl(int_literal_storage_, id);
        default:
            return erase_impl(fallback_literal_storage_, id);
    }
}
bool ReferenceNodeStorageBackend::erase_bnode([[maybe_unused]] identifier::NodeID id) {
    return erase_impl(bnode_storage_, id);
}
bool ReferenceNodeStorageBackend::erase_variable([[maybe_unused]] identifier::NodeID id) {
    return erase_impl(variable_storage_, id);
}

}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage