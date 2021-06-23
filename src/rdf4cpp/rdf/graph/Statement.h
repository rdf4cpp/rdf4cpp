#ifndef RDF4CPP_STATEMENT_H
#define RDF4CPP_STATEMENT_H

#include <rdf4cpp/rdf/graph/TriplePattern.h>

namespace rdf4cpp::rdf::graph {

class Statement : public TriplePattern {
public:
    Statement() = default;

    Statement(RDFNode subject, RDFNode predicate, RDFNode object);

    [[nodiscard]] bool valid() const;
};
}  // namespace rdf4cpp::rdf::graph
#endif  //RDF4CPP_STATEMENT_H
