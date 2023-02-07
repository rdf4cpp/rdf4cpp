#ifndef RDF4CPP_INODESCOPE_HPP
#define RDF4CPP_INODESCOPE_HPP

#include <rdf4cpp/rdf/bnode_management/INodeFactory.hpp>
#include <rdf4cpp/rdf/util/SharedPtr.hpp>

#include <optional>
#include <string_view>

namespace rdf4cpp::rdf::util {

struct INodeScope : virtual INodeFactory {
    [[nodiscard]] virtual storage::node::identifier::NodeBackendHandle find_node(std::string_view label) const noexcept = 0;
    [[nodiscard]] virtual std::optional<std::string_view> find_label(storage::node::identifier::NodeBackendHandle handle) const noexcept = 0;
    virtual void label_node(std::string_view label, storage::node::identifier::NodeBackendHandle handle) = 0;

    [[nodiscard]] virtual SharedPtr<INodeScope> subscope(std::string scope_name) noexcept = 0;
};

}  //namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_INODESCOPE_HPP
