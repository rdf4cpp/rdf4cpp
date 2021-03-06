# Storage

In rdf4cpp, we try our best to spare the user the details of how graphs and RDF nodes are actually stored in the backend. 
So if you just want to use rdf4cpp, you most probably do not need to go down this rabbit hole. You can just use the defaults.  

Storage is split up into two parts, [`NodeStorage`](./node) and [tuple storage \(`DatasetStorage`\)](./tuple).
For both storage parts, reference implementations are provided and used as a sensible default. 

## NodeStorage
`NodeStorage`s store information about RDF `Node`s. By default, there is only one default NodeStorage which is automatically used. 
The constructors of `BlankNode`, `IRI`, `Literal` and `Variable` optionally allow to use another `NodeStorage`. 
The `ReferenceNodeStorageBackend` implementation of the `NodeStorage` backend `INodeStorageBackend` which is used by default is thread-safe.
For more details, consult [rdf4cpp/rdf/storage/node/REAMDE.md](./node/README.md).

## Tuple Storage
Tuple Storage is still WIP. 
A first draft can be found at [rdf4cpp/rdf/storage/tuple/](./tuple/).
Implementation may be subject to future change. 