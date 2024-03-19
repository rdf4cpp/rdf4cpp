# Node Storage

In rdf4cpp, we try our best to spare the user the details of how graphs and RDF nodes are actually stored in the backend.
So if you just want to use rdf4cpp, you most probably do not need to go down this rabbit hole. You can just use the defaults.

`NodeStorage`s store information about RDF `Node`s. By default, there is only one default NodeStorage which is automatically used
(it lives at `rdf4cpp::storage::default_node_storage`).
The constructors of `BlankNode`, `IRI`, `Literal` and `Variable` optionally allow to use another `NodeStorage`.
The `SyncReferenceNodeStorage` implementation of the `NodeStorage` concept, which is used by default, is thread-safe.
Whereas the `UnsyncReferenceNodeStorage` implementation is not.

- `NodeStorage` is the central concept that defines what a node storage must be able to do.
- `NodeStorageVTable` is a vtable for a NodeStorage. It can be generated from any class that is a `NodeStorage`.
- `DynNodeStoragePtr` is a non-owning pointer to any `NodeStorage`, it stores an instance-pointer and a vtable-pointer.
- Identifiers for `Node`s and their properties are found in [identifier](identifier/README.md)
- Two reference implementation based on `dice::sparse_map` are provided, one is threadsafe the other is not, more details
  at [reference_node_storage](reference_node_storage)
- [view](view/README.md) contains view classes to access information about a node stored in an implementation-specific
  backend. 

## Requirements for `NodeStorage` implementations
- Implementations must **not** assume that all nodes are stored in the node storage,
  in fact small `Literals` that can fit in 42 bits (e.g. values of `xsd:int`) are usually **not** stored there
- `IRIs`, `BlankNodes` and `Variables` should usually be stored
  as their string representations
- `Literals` can be stored either as their canonical lexical form or as values
  - when trying to store `Literals` as values the implementation is
    required to accurately report which `Literal` types can be stored as
    values via `NodeStorage::has_specialized_storage_for`. **Inaccurate
    reporting will result in undefined behaviour.**
  - Warning: `Literals` that have value storage are assumed to never need escaping when converting them to their 
    n-triples string representation. I.e. the output of their corresponding `to_string` function will be used
    without escaping it.
  - see [reference_node_storage](reference_node_storage) for details
- Some `IRIs` are reserved by default (see [reserved_datatype_ids](../datatypes/registry/FixedIdMappings.hpp))
  these `IRIs` **must always** be present in the `NodeStorage` 
  and assigned the given `NodeID`. **Not upholding this invariant results in undefined behaviour.
  See [SyncReferenceNodeStorage::SyncReferenceNodeStorage()](reference_node_storage/SyncReferenceNodeStorage.cpp)
  for the expected usage.**

## General Notes for Implementors
- **Passing a `Literal` that is supposed to be stored in lexical storage (as required by the `NodeStorage` implementation) 
  as a value view results in undefined behaviour.**
- **Passing a `Literal` that is supposed to be stored in value storage (as required by the `NodeStorage` implementation)
  as a lexical view results in undefined behaviour.**
