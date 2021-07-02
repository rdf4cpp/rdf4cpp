#ifndef RDF4CPP_STATEMENT_HPP
#define RDF4CPP_STATEMENT_HPP

#include <rdf4cpp/rdf/query/TriplePattern.hpp>

namespace rdf4cpp::rdf {

class Statement : public query::TriplePattern {
    // TODO: adjust API to Quad
public:
    Statement() = default;

    Statement(Node subject, Node predicate, Node object);

    [[nodiscard]] bool valid() const;
};
}  // namespace rdf4cpp::rdf
#endif  //RDF4CPP_STATEMENT_HPP
