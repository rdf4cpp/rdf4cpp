#ifndef RDF4CPP_NODE_HPP
#define RDF4CPP_NODE_HPP

/** @file
 * @brief RDFNode
 * Long
 */

#include <optional>
#include <string>

#include <rdf4cpp/rdf/storage/node/BackendNodeHandle.hpp>

/**
 * Namespace docu
 */
namespace rdf4cpp::rdf {
class Literal;
class BlankNode;
class IRI;
namespace query {
class Variable;
};
/**
 * @brief RDFNode models IRI, Literal, BlankNode and Variable.
 */
class Node {
protected:
    using BackendNodeHandle = rdf4cpp::rdf::storage::node::BackendNodeHandle;
    using NodeID = rdf4cpp::rdf::storage::node::NodeID;
    using NodeStorage = rdf4cpp::rdf::storage::node::NodeStorage;
    using RDFNodeType = rdf4cpp::rdf::storage::node::RDFNodeType;
    BackendNodeHandle handle_;

    explicit Node(NodeID id);
    explicit Node(const BackendNodeHandle &id);

public:
    /**
     * Default construction produces null() const RDFNode. This node models an unset or invalid RDFNode.
     */
    Node() = default;

    [[nodiscard]] std::string as_string(bool quoting = false) const;

    /**
     * Checks weather the node is a Literal. If yes, it is safe to convert it with `auto literal = (Literal) rdf_node;`
     * @return if this is a Literal
     */
    [[nodiscard]] bool is_literal() const;

    /**
     * Checks weather the node is a Variable. If yes, it is safe to convert it with `auto variable = (Variable) rdf_node;`
     * @return if this is a Variable
     */
    [[nodiscard]] bool is_variable() const;

    /**
     * Checks weather the node is a BlankNode. If yes, it is safe to convert it with `auto bnode = (BlankNode) rdf_node;`
     * @return if this is a BlankNode
     */
    [[nodiscard]] bool is_bnode() const;

    /**
     * Checks weather the node is a IRI. If yes, it is safe to convert it with `auto iri = (Literal) rdf_node;`
     * @return if this is a IRI
     */
    [[nodiscard]] bool is_iri() const;


    [[nodiscard]] RDFNodeType type() const;

    bool operator==(const Node &other) const;

    std::strong_ordering operator<=>(const Node &other) const;

    explicit operator BlankNode() const;
    explicit operator IRI() const;
    explicit operator Literal() const;
    explicit operator query::Variable() const;


    /**
     * Check weather the has no value are an invalid value.
     * @return if value is unset or invalid
     */
    [[nodiscard]] bool null() const noexcept;
};
}  // namespace rdf4cpp::rdf

#include <rdf4cpp/rdf/BlankNode.hpp>
#include <rdf4cpp/rdf/IRI.hpp>
#include <rdf4cpp/rdf/Literal.hpp>
#include <rdf4cpp/rdf/query/Variable.hpp>

#endif  //RDF4CPP_NODE_HPP
