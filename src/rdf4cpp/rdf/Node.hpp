#ifndef RDF4CPP_NODE_HPP
#define RDF4CPP_NODE_HPP

/** @file
 * @brief RDFNode
 * Long
 */

#include <rdf4cpp/rdf/storage/node/BackendNodeHandle.hpp>

#include <optional>
#include <string>

/**
 * In this namespace the implementations of core RDF Concepts are defined. Class names are equal to terms defined in <a href="https://www.w3.org/TR/2014/REC-rdf11-concepts-20140225/#resources-and-statements">RDF 1.1 Concepts and Abstract Syntax"</a>.
 */
namespace rdf4cpp::rdf {
class Literal;
class BlankNode;
class IRI;
namespace query {
class Variable;
};
/**
 * @brief Models a node in RDF <span>Dataset</span>s, RDF <span>Graphs</span>s or pattern matching tuples like <span>QuadPattern</span>s or <span>TriplePattern</span>s.
 * <p><b>Please note:</b> The edges of an RDF Graph, dubbed <span>Predicate</span>s, are <span>IRI</span>s. As such the same resource can also be used as a node.
 * For the sake of simplicity, we decided to have no separate class for edges in an RDF graph.
 * You can determine if a Node is an edge by the the fact that it is used as predicate in a Dataset, Graph, Quad, Statement, QuadPattern or TriplePattern.</p>
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
     * Default construction produces null() const Node. This node models an unset or invalid Node.
     * null() const <span>Node</span>s should only be used as temporary placeholders. They cannot be inserted into a Graph or Dataset.
     */
    Node() = default;

    /**
     * Returns a string representation of the given node in N-format as defined by <a href="https://www.w3.org/TR/n-triples/">N-Triples</a> and <a href="https://www.w3.org/TR/n-quads/">N-Quads</a>.
     * @return string representation in N-format
     */
    [[nodiscard]] operator std::string() const;

    /**
     * @see operator std::string() const
     * @param os
     * @param node
     * @return
     */
    friend std::ostream &operator<<(std::ostream &os, const Node &node);

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

    // TODO: remove?
    [[nodiscard]] RDFNodeType type() const;

    bool operator==(const Node &other) const;

    std::strong_ordering operator<=>(const Node &other) const;

    /**
     * Conversion to BlankNode is only safe if `(is_bnode() == true)`
     * @return a copy of type BlankNode
     */
    explicit operator BlankNode() const;
    /**
     * Conversion to IRI is only safe if `(is_iri() == true)`
     * @return a copy of type IRI
     */
    explicit operator IRI() const;
    /**
     * Conversion to Literal is only safe if `(is_literal() == true)`
     * @return a copy of type Literal
     */
    explicit operator Literal() const;
    /**
     * Conversion to Variable is only safe if `(is_variable() == true)`
     * @return a copy of type Variable
     */
    explicit operator query::Variable() const;


    /**
     * Check whether the Node has no value or an invalid value.
     * @return if value is unset or invalid
     */
    [[nodiscard]] bool null() const noexcept;
};
}  // namespace rdf4cpp::rdf

#include <ostream>
#include <rdf4cpp/rdf/BlankNode.hpp>
#include <rdf4cpp/rdf/IRI.hpp>
#include <rdf4cpp/rdf/Literal.hpp>
#include <rdf4cpp/rdf/query/Variable.hpp>

#endif  //RDF4CPP_NODE_HPP
