#ifndef RDF4CPP_IRIFACTORY_HPP
#define RDF4CPP_IRIFACTORY_HPP

#include <nonstd/expected.hpp>
#include <string>
#include <string_view>

#include <dice/sparse-map/sparse_map.hpp>

#include <rdf4cpp/rdf/IRI.hpp>

namespace rdf4cpp::rdf {
enum class IRIFactoryError {
    UnknownPrefix,
};

class IRIFactory {
    dice::sparse_map::sparse_map<std::string, std::string, dice::hash::DiceHashwyhash<std::string_view>, std::equal_to<>> prefixes;

public:
    std::string base;

    explicit IRIFactory(std::string_view base = "http://example.org/") noexcept;

    [[nodiscard]] nonstd::expected<IRI, IRIFactoryError> from_relative(std::string_view rel, storage::node::NodeStorage &storage = storage::node::NodeStorage::default_instance()) const noexcept;
    [[nodiscard]] nonstd::expected<IRI, IRIFactoryError> from_prefix(std::string_view prefix, std::string_view local, storage::node::NodeStorage &storage = storage::node::NodeStorage::default_instance()) const;

    void assign_prefix(std::string_view prefix, std::string_view expanded);
    void clear_prefix(std::string_view prefix);

private:
    [[nodiscard]] static bool is_relative(std::string_view iri) noexcept;
    [[nodiscard]] nonstd::expected<IRI, IRIFactoryError> create_and_validate(std::string_view iri, storage::node::NodeStorage &storage) const noexcept;
};
}  // namespace rdf4cpp::rdf

#endif  //RDF4CPP_IRIFACTORY_HPP
