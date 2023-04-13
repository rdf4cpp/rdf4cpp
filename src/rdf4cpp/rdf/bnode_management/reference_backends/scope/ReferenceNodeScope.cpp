#include <rdf4cpp/rdf/bnode_management/reference_backends/scope/ReferenceNodeScope.hpp>

namespace rdf4cpp::rdf::util {

ReferenceNodeScope::ReferenceNodeScope(ReferenceNodeScope const &other) : label_to_storage{other.label_to_storage} {}

ReferenceNodeScope::ReferenceNodeScope(ReferenceNodeScope &&other) noexcept : label_to_storage{std::move(other.label_to_storage)},
                                                                              storage_to_label{std::move(other.storage_to_label)} {
}

storage::node::identifier::NodeBackendHandle ReferenceNodeScope::find_node(std::string_view label) const noexcept {
    std::shared_lock lock{this->mutex};

    auto it = this->label_to_storage.find(label);
    if (it == this->label_to_storage.end()) {
        return storage::node::identifier::NodeBackendHandle{};
    }

    return it->second;
}

std::optional<std::string_view> ReferenceNodeScope::find_label(storage::node::identifier::NodeBackendHandle handle) const noexcept {
    std::shared_lock lock{this->mutex};

    if (auto it = this->storage_to_label.find(handle); it != this->storage_to_label.end()) {
        return it->second;
    }

    return std::nullopt;
}

void ReferenceNodeScope::label_node(std::string_view label, storage::node::identifier::NodeBackendHandle handle) {
    std::unique_lock lock{this->mutex};

    auto const [it, inserted] = this->label_to_storage.emplace(label, handle);
    assert(inserted);

    auto const [_, inserted2] = this->storage_to_label.emplace(handle, it->first);
    assert(inserted2);
}

} // namespace rdf4cpp::rdf::util