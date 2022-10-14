#ifndef RDF4CPP_PARSER_PARSINGERROR_HPP
#define RDF4CPP_PARSER_PARSINGERROR_HPP

#include <cstdint>
#include <ostream>
#include <string>

namespace rdf4cpp::rdf::parser {

/**
 * An error that might occur while parsing a turtle file
 */
struct ParsingError {
    enum struct Type : uint8_t {
        Internal = 0,
        EofReached,
        BadSyntax,
        BadIri,
        BadCurie,
        BadLiteral,
        BadBlankNode,
    };

    Type error_type;
    uint64_t line;
    uint64_t col;
    std::string message;

    inline friend std::ostream &operator<<(std::ostream &os, ParsingError::Type const &err_t) noexcept {
        switch (err_t) {
            case Type::Internal:
                os << "internal error";
                break;
            case Type::EofReached:
                os << "unexpected end of file";
                break;
            case Type::BadSyntax:
                os << "bad syntax";
                break;
            case Type::BadIri:
                os << "bad iri";
                break;
            case Type::BadCurie:
                os << "bad curie";
                break;
            case Type::BadLiteral:
                os << "bad literal";
                break;
            case Type::BadBlankNode:
                os << "bad blank node";
                break;
        }

        return os;
    }

    inline friend std::ostream &operator<<(std::ostream &os, ParsingError const &error) noexcept {
        os << error.line << ':' << error.col << '(' << error.error_type << "): " << error.message;
        return os;
    }
};

} // namespace rdf4cpp::rdf::parser

#endif  //RDF4CPP_PARSER_PARSINGERROR_HPP
