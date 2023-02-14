# Node Storage

- `NodeStorage` is the central proxy class for `Node` storage backend.
- `INodeStorage` is an abstract class that node storage backends must implement.
- Identifiers for `Node`s and their properties are found in [identifier](./identifier/README.md)
- A reference implementation of a thread-safe node storage backend based on `tsl::sparse_map` is provided
  at [reference_node_storage](./reference_node_storage)
- [view](./view/README.md) contains proxy classes to access information about a node stored in an implementation-specific
  backend. 

## Requirements for `INodeStorage` implementations

- Implementations must not assume that all nodes are stored in the node storage,
  in fact small `Literals` that can fit in 42 bits (e.g. values of `xsd:int`) are usually _not_ stored there
- `IRIs`, `BlankNodes` and `Variables` should usually be stored
  as their string representations
- `Literals` can be stored either as their canonical lexical form or as values
  - when trying to store `Literals` as values the implementation is
    required to accurately report which `Literal` types can be stored as
    values via `INodeStorageBackend::has_specialized_storage_for`. Inaccurate
    reporting will result in undefined behaviour.
  - see [reference_node_storage](./reference_node_storage) for details
- Some `IRIs` are reserved by default (see [reserved_datatype_ids](../../datatypes/registry/FixedIdMappings.hpp))
  these `IRIs` _must always_ be present in the `NodeStorage` 
  and assigned the given `NodeID`. Not upholding this invariant results in undefined behaviour.
  See [ReferenceNodeStorage::ReferenceNodeStorage()](reference_node_storage/ReferenceNodeStorageBackend.cpp)
  for the expected usage.
