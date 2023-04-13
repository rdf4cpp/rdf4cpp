#ifndef RDF4CPP_RDF_BNODEMANAGEMENT_REFERENCEBACKENDS_SCOPE_REFERENCESCOPEBASE_HPP
#define RDF4CPP_RDF_BNODEMANAGEMENT_REFERENCEBACKENDS_SCOPE_REFERENCESCOPEBASE_HPP

#include <rdf4cpp/rdf/bnode_management/INodeScope.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/NodeBackendHandle.hpp>
#include <rdf4cpp/rdf/storage/util/robin-hood-hashing/robin_hood_hash.hpp>
#include <rdf4cpp/rdf/storage/util/tsl/sparse_map.h>

#include <memory>
#include <shared_mutex>

namespace rdf4cpp::rdf::util {

struct ReferenceNodeScope : INodeScope {
private:
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

private:
    std::shared_mutex mutable mutex; // protects label_to_storage and storage_to_label

    storage::util::tsl::sparse_map<PinnedString,
                                   storage::node::identifier::NodeBackendHandle,
                                   storage::util::robin_hood::hash<std::string_view>,
                                   std::equal_to<>> label_to_storage;

    storage::util::tsl::sparse_map<storage::node::identifier::NodeBackendHandle,
                                   std::string_view,
                                   storage::util::robin_hood::hash<storage::node::identifier::NodeBackendHandle>> storage_to_label;

    ReferenceNodeScope(ReferenceNodeScope const &other);
public:
    ReferenceNodeScope() = default;
    ReferenceNodeScope(ReferenceNodeScope &&other) noexcept;

    [[nodiscard]] storage::node::identifier::NodeBackendHandle find_node(std::string_view label) const noexcept override;
    [[nodiscard]] std::optional<std::string_view> find_label(storage::node::identifier::NodeBackendHandle handle) const noexcept override;
    void label_node(std::string_view label, storage::node::identifier::NodeBackendHandle handle) override;
};

}  //namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_RDF_BNODEMANAGEMENT_REFERENCEBACKENDS_SCOPE_REFERENCESCOPEBASE_HPP
