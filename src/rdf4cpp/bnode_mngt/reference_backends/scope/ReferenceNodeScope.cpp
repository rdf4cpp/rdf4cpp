#include <rdf4cpp/bnode_mngt/reference_backends/scope/ReferenceNodeScope.hpp>

namespace rdf4cpp::bnode_mngt {

ReferenceNodeScope::ReferenceNodeScope(ReferenceNodeScope const &other) : label_to_handle_{other.label_to_handle_} {}

ReferenceNodeScope::ReferenceNodeScope(ReferenceNodeScope &&other) noexcept : label_to_handle_{std::move(other.label_to_handle_)},
                                                                              handle_to_label_{std::move(other.handle_to_label_)} {
}

storage::identifier::NodeBackendHandle ReferenceNodeScope::find_node(std::string_view label) const noexcept {
    std::shared_lock lock{this->mutex_};

    if (auto it = this->label_to_handle_.find(label); it != this->label_to_handle_.end()) {
        return it->second;
    }

    return storage::identifier::NodeBackendHandle{};
}

std::optional<std::string_view> ReferenceNodeScope::find_label(storage::identifier::NodeBackendHandle handle) const noexcept {
    std::shared_lock lock{this->mutex_};

    if (auto it = this->handle_to_label_.find(handle); it != this->handle_to_label_.end()) {
        return it->second;
    }

    return std::nullopt;
}

void ReferenceNodeScope::label_node(std::string_view label, storage::identifier::NodeBackendHandle handle) {
    std::unique_lock lock{this->mutex_};

    auto const [it, inserted] = this->label_to_handle_.emplace(label, handle);
    if (!inserted) {
        throw std::invalid_argument{"Node already labeled"};
    }

    auto const [_it, inserted2] = this->handle_to_label_.emplace(handle, it->first);
    (void) _it;
    assert(inserted2);
}

} // namespace rdf4cpp::bnode_mngt