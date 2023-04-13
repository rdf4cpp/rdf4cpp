#ifndef RDF4CPP_BLANKNODEIDMANAGEMENT_METALL_COMMON_HPP
#define RDF4CPP_BLANKNODEIDMANAGEMENT_METALL_COMMON_HPP

#include <rdf4cpp/rdf.hpp>
#include <charconv>
#include <limits>

#include <metall/metall.hpp>
#include <metall/container/string.hpp>
#include <metall/container/unordered_map.hpp>

template<typename T>
using allocator_type = metall::manager::allocator_type<T>;

template<typename T>
using pointer = typename allocator_type<T>::pointer;

inline constexpr auto generator_name = "generator";
inline constexpr auto scope_name = "scope";

using namespace rdf4cpp::rdf;
using namespace rdf4cpp::rdf::util;

struct PersistableGenerator {
    size_t number = 0;

    size_t max_generated_id_size() const noexcept {
        return std::numeric_limits<size_t>::digits10;
    }

    char *generate_to_buf(char *buf) {
        auto res = std::to_chars(buf, buf + max_generated_id_size(), number++);
        assert(res.ec == std::errc{});
        return res.ptr;
    }
};

struct PersistableGeneratorFrontend : IIdGenerator {
    PersistableGenerator *impl;

    explicit PersistableGeneratorFrontend(PersistableGenerator *impl) : impl{impl} {}

    size_t max_generated_id_size() const noexcept override {
        return impl->max_generated_id_size();
    }

    char *generate_to_buf(char *buf) override {
        return impl->generate_to_buf(buf);
    }
};

struct PersistableScope {
private:
    using metall_string = metall::container::basic_string<char, std::char_traits<char>, allocator_type<char>>;

    allocator_type<std::byte> alloc;

    metall::container::unordered_map<metall_string,
                       storage::node::identifier::NodeBackendHandle,
                       storage::util::robin_hood::hash<std::string_view>,
                       std::equal_to<>,
                       allocator_type<std::pair<metall_string const, storage::node::identifier::NodeBackendHandle>>> label_to_storage;

    metall::container::unordered_map<storage::node::identifier::NodeBackendHandle,
                       typename allocator_type<char>::const_pointer,
                       storage::util::robin_hood::hash<storage::node::identifier::NodeBackendHandle>,
                       std::equal_to<>,
                       allocator_type<std::pair<storage::node::identifier::NodeBackendHandle const, typename allocator_type<char>::const_pointer>>> storage_to_label;
public:
    explicit PersistableScope(allocator_type<std::byte> alloc) : alloc{alloc},
                                                                 label_to_storage{alloc},
                                                                 storage_to_label{alloc} {}


    [[nodiscard]] std::optional<std::string_view> find_label(storage::node::identifier::NodeBackendHandle handle) const noexcept {
        if (auto it = this->storage_to_label.find(handle); it != this->storage_to_label.end()) {
            return std::string_view{std::to_address(it->second), std::strlen(std::to_address(it->second))};
        }

        return std::nullopt;
    }

    [[nodiscard]] storage::node::identifier::NodeBackendHandle find_node(std::string_view label) const noexcept {
        if (auto it = this->label_to_storage.find(metall_string{label, alloc}); it != this->label_to_storage.end()) {
            return it->second;
        }

        return storage::node::identifier::NodeBackendHandle{};
    }

    void label_node(std::string_view label, storage::node::identifier::NodeBackendHandle handle) {
        metall_string lab{label, alloc};
        auto const [it, inserted] = this->label_to_storage.emplace(lab, handle);
        assert(inserted);

        auto const [_, inserted2] = this->storage_to_label.emplace(handle, it->first.c_str());
        assert(inserted2);
    }
};

struct PersistableScopeFrontent : INodeScope {
    PersistableScope *impl;
    explicit PersistableScopeFrontent(PersistableScope *impl) : impl{impl} {}

    std::optional<std::string_view> find_label(storage::node::identifier::NodeBackendHandle handle) const noexcept override {
        return impl->find_label(handle);
    }

    storage::node::identifier::NodeBackendHandle find_node(std::string_view label) const noexcept override {
        return impl->find_node(label);
    }

    void label_node(std::string_view label, storage::node::identifier::NodeBackendHandle handle) override {
        return impl->label_node(label, handle);
    }
};

#endif  //RDF4CPP_BLANKNODEIDMANAGEMENT_METALL_COMMON_HPP
