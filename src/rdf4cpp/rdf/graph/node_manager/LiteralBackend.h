#ifndef RDF4CPP_LITERALBACKEND_H
#define RDF4CPP_LITERALBACKEND_H

#include "ID.h"
#include <string>
namespace rdf4cpp::rdf::graph::node_manager {

class LiteralBackend {
    std::string lexical;
    ID data_type;
    std::string lang_tag;

public:
    LiteralBackend(std::string lexical, const ID &dataType, std::string langTag = "");
    auto operator<=>(const LiteralBackend &) const = default;
    std::weak_ordering operator<=>(LiteralBackend const *other) const;

    [[nodiscard]] std::string as_string(bool quoting) const;
};
}  // namespace rdf4cpp::rdf::graph::node_manager

#endif  //RDF4CPP_LITERALBACKEND_H
