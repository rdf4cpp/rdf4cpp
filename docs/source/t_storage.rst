Storage
=======

.. toctree::

    t_node_storage
    t_NodeBackendHandle
    t_node_storage_view

In rdf4cpp, we try our best to spare the user the details of how graphs and RDF nodes are actually stored in the backend.
So if you just want to use rdf4cpp, you most probably do not need to go down this rabbit hole. You can just use the defaults.

Storage is split up into two parts, :doc:`node_storage` and *tuple storage (DatasetStorage)*.
For both storage parts, reference implementations are provided and used as a sensible default.

NodeStorage
___________

`NodeStorage`\ s store information about RDF `Node`\ s. By default, there is only one default NodeStorage which is automatically used.
The constructors of `BlankNode`, `IRI`, `Literal` and `Variable` optionally allow to use another `NodeStorage`.
The `ReferenceNodeStorageBackend` implementation of the `NodeStorage` backend `INodeStorageBackend` which is used by default is thread-safe.
For more details, consult :doc:`node_storage`.

Tuple Storage
_____________

Tuple Storage is still WIP.
A first draft can be found at :ref:`namespace_rdf4cpp__rdf__storage__tuple`.
Implementation may be subject to future change.