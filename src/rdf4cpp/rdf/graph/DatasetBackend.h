#ifndef RDF4CPP_DATASETBACKEND_H
#define RDF4CPP_DATASETBACKEND_H

#include <algorithm>
#include <rdf4cpp/rdf/graph/PatternSolutions.h>
#include <rdf4cpp/rdf/graph/Quad.h>
#include <rdf4cpp/rdf/graph/QuadPattern.h>
#include <rdf4cpp/rdf/node/all.h>
#include <set>
namespace rdf4cpp::rdf::graph {


class DatasetBackend {
public:
    virtual ~DatasetBackend() = 0;
    virtual void add(const Quad &quad) = 0;
    [[nodiscard]] virtual bool contains(const Quad &quad) const = 0;
    [[nodiscard]] virtual size_t size() const = 0;
    [[nodiscard]] virtual PatternSolutions match(const QuadPattern &quad_pattern) const = 0;
    [[nodiscard]] virtual size_t size(const node::IRIResource &graph_name) const = 0;
    [[nodiscard]] virtual std::string as_string() const = 0;
};


}  // namespace rdf4cpp::rdf::graph


#endif  //RDF4CPP_DATASETBACKEND_H
