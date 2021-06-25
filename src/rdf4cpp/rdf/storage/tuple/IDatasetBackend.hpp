#ifndef RDF4CPP_IDATASETBACKEND_HPP
#define RDF4CPP_IDATASETBACKEND_HPP

#include <rdf4cpp/rdf/Quad.hpp>
#include <rdf4cpp/rdf/query/PatternSolutions.hpp>
#include <rdf4cpp/rdf/query/QuadPattern.hpp>

#include <algorithm>
#include <set>

namespace rdf4cpp::rdf::storage::tuple {


class IDatasetBackend {
private:
    using PatternSolutions = rdf4cpp::rdf::query::PatternSolutions;
    using QuadPattern = rdf4cpp::rdf::query::QuadPattern;

public:
    virtual ~IDatasetBackend() = 0;
    virtual void add(const Quad &quad) = 0;
    [[nodiscard]] virtual node::NodeStorage &node_storage() const = 0;
    [[nodiscard]] virtual bool contains(const Quad &quad) const = 0;
    [[nodiscard]] virtual size_t size() const = 0;
    [[nodiscard]] virtual PatternSolutions match(const QuadPattern &quad_pattern) const = 0;
    [[nodiscard]] virtual size_t size(const IRI &graph_name) const = 0;
    [[nodiscard]] virtual std::string as_string() const = 0;
};


}  // namespace rdf4cpp::rdf::storage::tuple


#endif  //RDF4CPP_IDATASETBACKEND_HPP
