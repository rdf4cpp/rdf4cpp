Storage
=======

.. toctree::

    t_node_storage
    t_NodeBackendHandle
    t_node_storage_view

In rdf4cpp, we try our best to spare the user the details of how graphs and RDF nodes are actually stored in the backend.
So if you just want to use rdf4cpp, you most probably do not need to go down this rabbit hole. You can just use the defaults.

Storage is split up into two parts, :doc:`t_node_storage` and *tuple storage (DatasetStorage)*.
For both storage parts, reference implementations are provided and used as a sensible default.

NodeStorage
___________

`NodeStorage`\s store information about RDF `Node`\s. By default, there is only one default NodeStorage which is automatically used
(it lives at `rdf4cpp::storage::default_node_storage`).
The constructors of `BlankNode`, `IRI`, `Literal` and `Variable` optionally allow to use another `NodeStorage`.
The `SyncReferenceNodeStorage` implementation of the `NodeStorage` concept, which is used by default, is thread-safe.
Whereas the `UnsyncReferenceNodeStorage` implementation is not.
For more details, consult :doc:`t_node_storage`.
