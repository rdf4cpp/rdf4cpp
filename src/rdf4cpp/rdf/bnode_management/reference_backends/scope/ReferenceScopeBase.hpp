#ifndef RDF4CPP_RDF_BNODEMANAGEMENT_REFERENCEBACKENDS_SCOPE_REFERENCESCOPEBASE_HPP
#define RDF4CPP_RDF_BNODEMANAGEMENT_REFERENCEBACKENDS_SCOPE_REFERENCESCOPEBASE_HPP

#include <rdf4cpp/rdf/bnode_management/INodeScope.hpp>
#include <rdf4cpp/rdf/storage/node/NodeStorage.hpp>
#include <rdf4cpp/rdf/storage/util/robin-hood-hashing/robin_hood_hash.hpp>
#include <rdf4cpp/rdf/storage/util/tsl/sparse_map.h>
#include <rdf4cpp/rdf/util/SharedPtr.hpp>

#include <memory>
#include <shared_mutex>
#include <string>

namespace rdf4cpp::rdf::util {

template<typename Self>
struct ReferenceScopeBase : INodeScope, EnableSharedFromThis<Self> {
protected:
    struct PinnedString {
        std::unique_ptr<char[]> data;
        size_t size;

        inline explicit PinnedString(std::string_view s) : data{std::make_unique<char[]>(s.size())},
                                                           size{s.size()} {
            std::copy(s.begin(), s.end(), data.get());
        }

        PinnedString(PinnedString &&other) noexcept = default;
        inline PinnedString(PinnedString const &other) : data{std::make_unique<char[]>(other.size)},
                                                         size{other.size} {
            std::copy(other.data.get(), other.data.get() + other.size, this->data.get());
        }

        inline operator std::string_view() const noexcept {
            return std::string_view{this->data.get(), this->size};
        }

        inline bool operator==(PinnedString const &other) const noexcept {
            return static_cast<std::string_view>(*this) == static_cast<std::string_view>(other);
        }

        inline bool operator==(std::string_view const s) const noexcept {
            return static_cast<std::string_view>(*this) == s;
        }
    };

protected:
    std::shared_mutex mutable mutex; // protects only label_to_storage and storage_to_label

    storage::util::tsl::sparse_map<PinnedString,
                                   storage::node::identifier::NodeBackendHandle,
                                   storage::util::robin_hood::hash<std::string_view>,
                                   std::equal_to<>> label_to_storage;

    storage::util::tsl::sparse_map<storage::node::identifier::NodeBackendHandle,
                                   std::string_view,
                                   storage::util::robin_hood::hash<storage::node::identifier::NodeBackendHandle>> storage_to_label;

    storage::util::tsl::sparse_map<std::string,
                                   SharedPtr<ReferenceScopeBase>,
                                   storage::util::robin_hood::hash<std::string_view>,
                                   std::equal_to<>> subscopes;

    void forward_mapping(std::string_view label, storage::node::identifier::NodeBackendHandle hnd) {
        for (auto &[_, child] : this->subscopes) {
            auto it = child->label_to_storage.find(label);
            if (it != child->label_to_storage.end()) {
                continue;
            }

            child->label_to_storage.emplace(label, hnd);
            child->forward_mapping(label, hnd);
        }
    }

    ReferenceScopeBase(ReferenceScopeBase const &other) : label_to_storage{other.label_to_storage} {}
public:
    ReferenceScopeBase() = default;
    ReferenceScopeBase(ReferenceScopeBase &&other) noexcept : label_to_storage{std::move(other.label_to_storage)},
                                                              storage_to_label{std::move(other.storage_to_label)},
                                                              subscopes{std::move(other.subscopes)} {
    }

    [[nodiscard]] storage::node::identifier::NodeBackendHandle find_node(std::string_view label) const noexcept override {
        std::shared_lock lock{this->mutex};

        auto it = this->label_to_storage.find(label);
        if (it == this->label_to_storage.end()) {
            return this->make_null_node();
        }

        return it->second;
    }

    [[nodiscard]] std::optional<std::string_view> find_label(storage::node::identifier::NodeBackendHandle handle) const noexcept override {
        std::shared_lock lock{this->mutex};

        if (auto it = this->storage_to_label.find(handle); it != this->storage_to_label.end()) {
            return it->second;
        }

        return std::nullopt;
    }

    void label_node(std::string_view label, storage::node::identifier::NodeBackendHandle handle) override {
        std::unique_lock lock{this->mutex};

        auto const [it, inserted] = this->label_to_storage.emplace(label, handle);
        assert(inserted);
        this->forward_mapping(label, handle);

        auto const [_, inserted2] = this->storage_to_label.emplace(handle, it->first);
        assert(inserted2);
    }

    [[nodiscard]] SharedPtr<INodeScope> subscope(std::string scope_name) noexcept override {
        if (scope_name.empty()) {
            return this->shared_from_this();
        }

        if (auto it = this->subscopes.find(scope_name); it != this->subscopes.end()) {
            return it->second;
        }

        auto [it, inserted] = this->subscopes.emplace(std::move(scope_name), SharedPtr<Self>::from_raw(new Self{static_cast<Self const &>(*this)}));
        assert(inserted);

        //for (auto const &[key, value] : this->label_to_storage) {
        //    it.value()->label_to_storage.emplace(static_cast<std::string_view>(key), value);
        //}

        return it->second;
    }
};

}  //namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_RDF_BNODEMANAGEMENT_REFERENCEBACKENDS_SCOPE_REFERENCESCOPEBASE_HPP
