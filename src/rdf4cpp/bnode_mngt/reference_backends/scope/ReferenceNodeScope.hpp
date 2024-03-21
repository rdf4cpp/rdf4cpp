#ifndef RDF4CPP_RDF_BNODEMANAGEMENT_REFERENCEBACKENDS_SCOPE_REFERENCESCOPEBASE_HPP
#define RDF4CPP_RDF_BNODEMANAGEMENT_REFERENCEBACKENDS_SCOPE_REFERENCESCOPEBASE_HPP

#include <rdf4cpp/bnode_mngt/INodeScope.hpp>
#include <rdf4cpp/storage/identifier/NodeBackendHandle.hpp>

#include <dice/sparse-map/sparse_map.hpp>
#include <dice/hash.hpp>

#include <memory>
#include <shared_mutex>

namespace rdf4cpp::bnode_mngt {

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
    std::shared_mutex mutable mutex_; // protects label_to_handle_ and handle_to_label_

    dice::sparse_map::sparse_map<PinnedString,
                                 storage::identifier::NodeBackendHandle,
                                 dice::hash::DiceHashwyhash<std::string_view>,
                                 std::equal_to<>>
            label_to_handle_;

    dice::sparse_map::sparse_map<storage::identifier::NodeBackendHandle,
                                 std::string_view>
            handle_to_label_;

    ReferenceNodeScope(ReferenceNodeScope const &other);
public:
    ReferenceNodeScope() = default;
    ReferenceNodeScope(ReferenceNodeScope &&other) noexcept;

    [[nodiscard]] storage::identifier::NodeBackendHandle find_node(std::string_view label) const noexcept override;
    [[nodiscard]] std::optional<std::string_view> find_label(storage::identifier::NodeBackendHandle handle) const noexcept override;
    void label_node(std::string_view label, storage::identifier::NodeBackendHandle handle) override;
};

}  //namespace rdf4cpp::bnode_mngt

#endif  //RDF4CPP_RDF_BNODEMANAGEMENT_REFERENCEBACKENDS_SCOPE_REFERENCESCOPEBASE_HPP
