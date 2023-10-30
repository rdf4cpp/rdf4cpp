#ifndef RDF4CPP_IRIFACTORY_HPP
#define RDF4CPP_IRIFACTORY_HPP

#include <nonstd/expected.hpp>
#include <string>
#include <string_view>
#include <map>

#include <rdf4cpp/rdf/IRI.hpp>

namespace rdf4cpp::rdf {
enum class IRIFactoryError {
    UnknownPrefix,
};

class IRIFactory {
    std::map<std::string, std::string> prefixes;

public:
    std::string base;

    explicit IRIFactory(std::string_view base = "http://example.org/") noexcept;

    [[nodiscard]] nonstd::expected<IRI, IRIFactoryError> from_relative(std::string_view rel, storage::node::NodeStorage &storage = storage::node::NodeStorage::default_instance()) const noexcept;
    [[nodiscard]] nonstd::expected<IRI, IRIFactoryError> from_absolute_prefix(std::string_view prefix, std::string_view local, storage::node::NodeStorage &storage = storage::node::NodeStorage::default_instance()) const;
    [[nodiscard]] nonstd::expected<IRI, IRIFactoryError> from_relative_prefix(std::string_view prefix, std::string_view local, storage::node::NodeStorage &storage = storage::node::NodeStorage::default_instance()) const;

    void assign_prefix(std::string_view prefix, std::string_view expanded);
    void clear_prefix(std::string_view prefix);

private:
    [[nodiscard]] nonstd::expected<IRI, IRIFactoryError> create_and_validate(std::string_view iri, storage::node::NodeStorage &storage) const noexcept;
};
}  // namespace rdf4cpp::rdf

#endif  //RDF4CPP_IRIFACTORY_HPP
