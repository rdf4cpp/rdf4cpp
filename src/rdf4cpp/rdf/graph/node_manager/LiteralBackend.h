#ifndef RDF4CPP_LITERALBACKEND_H
#define RDF4CPP_LITERALBACKEND_H

#include "rdf4cpp/rdf/graph/node_manager/ID.h"
#include <compare>
#include <string>

namespace rdf4cpp::rdf::graph::node_manager {

class LiteralBackend {
    std::string lexical;
    ID datatype_id_;
    std::string lang_tag;

public:
    LiteralBackend(std::string lexical, const ID &dataType, std::string langTag = "");
    auto operator<=>(const LiteralBackend &) const = default;
    std::weak_ordering operator<=>(LiteralBackend const *other) const;

    [[nodiscard]] std::string as_string(bool quoting) const;

    [[nodiscard]] const std::string &lexical_form() const {
        return lexical;
    }

    [[nodiscard]] const ID &datatype_id() const {
        return datatype_id_;
    }

    [[nodiscard]] const std::string &language_tag() const {
        return lang_tag;
    }
};
}  // namespace rdf4cpp::rdf::graph::node_manager

#endif  //RDF4CPP_LITERALBACKEND_H
