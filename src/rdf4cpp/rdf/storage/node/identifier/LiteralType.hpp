#ifndef RDF4CPP_LITERALTYPE_HPP
#define RDF4CPP_LITERALTYPE_HPP

#include <cstdint>


namespace rdf4cpp::rdf::storage::node::identifier {

/**
 * <p>A literal type specifies the type of a literal. Types, which are not available within this enum class MUST be specified as LiteralType::OTHER.</p>
 * <p>The purpose of this type is to provide a short-cut for common types like xsd:string or xsd:float when used within NodeID or NodeBackendHandle.
 * If a LiteralType instance is part of an identifier or handle for a Literal stored in NodeStorage, it must not contradict the type information stored therein. </p>
 */
enum class /*__attribute__((__packed__)) */ LiteralType : uint8_t {
    OTHER = 0,
    // ...
    // TODO: add XSD types and langString

};
}  // namespace rdf4cpp::rdf::storage::node::identifier

#endif  //RDF4CPP_LITERALTYPE_HPP
