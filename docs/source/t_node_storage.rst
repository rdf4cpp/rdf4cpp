Node Storage
############

 - `NodeStorage` is the central proxy class for `Node` storage backend.
 - `INodeStorage` is an abstract class that node storage backends must implement.
 - Identifiers for Nodes and their properties are found in :doc:`t_NodeBackendHandle`
 - A reference implementation of a thread-safe node storage backend based on `tsl::sparse_map` is provided
   at :class:`rdf4cpp::rdf::storage::node::reference_node_storage::ReferenceNodeStorageBackend`
 - :doc:`t_node_storage_view` contains proxy classes to access information about a node stored in an implementation-specific
   backend.

Requirements for `INodeStorage` Implementations
_______________________________________________

 - Implementations must not assume that all nodes are stored in the node storage,
   in fact small `Literals` that can fit in 42 bits (e.g. values of `xsd:int`) are usually *not* stored there
 - `IRIs`, `BlankNodes` and `Variables` should usually be stored
   as their string representations
 - `Literals` can be stored either as their canonical lexical form or as values
    - when trying to store `Literals` as values the implementation is
      required to accurately report which `Literal` types can be stored as
      values via :func:`rdf4cpp::rdf::storage::node::INodeStorageBackend::has_specialized_storage_for`. **Inaccurate
      reporting will result in undefined behaviour.**
    - Warning: `Literals` that have value storage are assumed to never need escaping when converting them to their
      n-triples string representation. I.e. the output of their corresponding `to_string` function will be used
      without escaping it.
    - see :class:`rdf4cpp::rdf::storage::node::reference_node_storage::ReferenceNodeStorageBackend` for details
 - Some `IRIs` are reserved by default (see :var:`rdf4cpp::rdf::datatypes::registry::reserved_datatype_ids`)
   these `IRIs` *must always* be present in the `NodeStorage`
   and assigned the given `NodeID`. **Not upholding this invariant results in undefined behaviour.
   See :class:`rdf4cpp::rdf::storage::node::reference_node_storage::ReferenceNodeStorageBackend`
   for the expected usage.**

General Notes for Implementors
______________________________

 - **Passing a `Literal` that is supposed to be stored in lexical storage (as required by the `INodeStorage` implementation)
   as a value view results in undefined behaviour.**
 - **Passing a `Literal` that is supposed to be stored in value storage (as required by the `INodeStorage` implementation)
   as a lexical view results in undefined behaviour.**
