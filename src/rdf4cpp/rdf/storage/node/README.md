# Node Storage

- `NodeStorage` is the central proxy class for `Node` storage backend.
- `INodeStorage` is an abstract class that node storage backends must implement.
- Identifiers for `Node`s and their properties are found in [identifier](./identifier/README.md)
- A reference implementation of a thread-safe node storage backend based on `tsl::sparse_map` is provided
  at [reference_node_storage](./reference_node_storage)
- [view](./view/README.md) contains proxy classes to access information about a node stored in an implementation-specific
  backend. 