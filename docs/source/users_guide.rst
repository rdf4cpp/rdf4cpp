Users Guide
===========

Terms and Definitions
---------------------
We use the official definitions of `<https://www.w3.org/TR/rdf11-concepts/>`_.

Framework Basics
----------------

The main types of rdf4cpp:

* :class:`rdf4cpp::rdf::Node`: Base class of Literal, IRI and BlankNode
* :class:`rdf4cpp::rdf::IRI`: An IRI.
* :class:`rdf4cpp::rdf::Literal`: A Literal containing some data.
* :class:`rdf4cpp::rdf::BlankNode`: A Blank Node used to connect other Nodes.
* :class:`rdf4cpp::rdf::Statement`: A basic rdf statement consisting of Subject, Predicate and Object.

Datatypes
---------

Literal supports common rdf Datatypes and operations on them (as defined in `<https://www.w3.org/TR/sparql12-query/#SparqlOps>`_),
as well as conversions between Literal Datatypes.

Parsing Files
-------------

The class :class:`rdf4cpp::rdf::parser::RDFFileParser` allows reading files containing rdf Statements and iterate over them.
Supported Formats: Turtle, TriG, NTriple and NQuad.
:class:`rdf4cpp::rdf::parser::IStreamQuadIterator` allows doing the same over arbitrary data streams.
