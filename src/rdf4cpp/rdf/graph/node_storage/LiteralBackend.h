#ifndef RDF4CPP_LITERALBACKEND_H
#define RDF4CPP_LITERALBACKEND_H

#include <rdf4cpp/rdf/graph/node_storage/NodeID.h>

#include <compare>
#include <string>

namespace rdf4cpp::rdf::graph::node_storage {

class LiteralBackend {
    NodeID datatype_id_;
    std::string lexical;
    std::string lang_tag;

public:
    [[nodiscard]] std::string quote_lexical() const;
    LiteralBackend(std::string lexical, const NodeID &dataType, std::string langTag = "");
    std::strong_ordering operator<=>(const LiteralBackend &) const noexcept;
    std::strong_ordering operator<=>(LiteralBackend const *other) const {
        return this <=> other;
    }

    bool operator==(const LiteralBackend &) const noexcept;

    [[nodiscard]] const std::string &lexical_form() const {
        return lexical;
    }

    [[nodiscard]] const NodeID &datatype_id() const {
        return datatype_id_;
    }

    [[nodiscard]] const std::string &language_tag() const {
        return lang_tag;
    }
};
}  // namespace rdf4cpp::rdf::graph::node_storage

#endif  //RDF4CPP_LITERALBACKEND_H
