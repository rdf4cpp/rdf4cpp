Identifiers Used to Store Nodes
===============================

`NodeBackendHandle` (64 bit)
____________________________

rdf4cpp uses `NodeBackendHandle` – a 64 bit wide type – to identify Nodes within a `NodeManager`.
The following figure gives an overview of its memory layout.

.. code-block:: none

    LSB                                                          MSB
    ┣━━━━━━━━━━━━━━━━ NodeBackendHandle (64 bit) ━━━━━━━━━━━━━━━━━━━┫
    ├────────────── NodeID (48 bit) ───────────────┤├┤├────────┤├┤├─┤
    ├───────── LiteralID (42 bit) ───────────┤├────┤^     ^     ^  ^
                                                ^   |     |     |  |
                                                |   |     |     |  free tagging bits (3 bit)
                              LiteralType (6 bit)   |     |     |
                                                    |     |     inlining tagging bit (1 bit)
                                                    |     |
                                                    |     NodeStorageID (10 bit)
                                                    |
                                                    NodeType (2 bit)

`NodeID` is the most frequently used identifier and is thus aligned with the LSB.
In the following, the parts of `NodeBackendHandle` will be explained starting from the most significant bits because the
parts in the less significant depend on the parts in the more significant bits.

Free Tagging Bits (3 bit)
-------------------------

The *free tagging bits* can be used by the internal storage to store 4 bit of information. Before calling any functions
on a `NodeBackendHandle` the *free tagging bits* must be reset to 0 again.

Inlinining tagging bit (1 bit)
------------------------------

The *inlinining tagging bit* indicates whether the value of the node is stored directly (in-place) inside the NodeID
instead of the node storage.

`NodeStorageID` (10 bit)
------------------------

The `NodeStorageID` identifies the `NodeStorage` in which the identified `Node` is stored.

`NodeType` (2 bit)
------------------

The `NodeType` specifies if the `Node` is a `BlankNode`, `IRI`, `Literal` or `Variable`.

`NodeID` (48 bit)
-----------------

The `NodeID` identifies a `Node` given `NodeType` and `NodeStorageID`. A `NodeID` of 0 is considered a null `NodeID`
which must not identify any resource.

For `Literal`s the `NodeID` splits up further into `LiteralType` and `LiteralID`.

`LiteralType` (6 bit)
+++++++++++++++++++++

The `LiteralType` is a shortcut for the type stored within the backend of the `Literal`.
To reduce computation costs, common types like xsd can be encoded directly into the NodeBackendHandle.
For all other types, `LiteralType` is set to `OTHER`.
In that case, the type information must be retrieved from backend if needed.

`LiteralID` (42 bit)
++++++++++++++++++++

The `LiteralID` identifies a literal given `NodeStorageID`, `NodeType==Literal` and `LiteralType`.

`LiteralID` with inlined language tag
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For `rdf::langString` the language tag can be inlined into the `LiteralID`.
This uses the uppermost bits of the `LiteralID`. The amount of bits used depends on the configuration (default 2).
Inlining uses only as many bits as it needs, the remaining bits of the `LiteralID` is used for storing the
truncated `LiteralID`
If you are operating on `LiteralID`\s directly, you need to check if they are inlined and possibly de-inline them before
using (the inlining tagging bit of the `NodeBackendHandle` will be set).

Important Notes
_______________

 - `NodeBackendHandle`\s depend on the initialization order of `NodeManager`\s and the creation order if `Node`\s within
   a `NodeManager`.

   - They are not guaranteed to be equal between two executions.
   - `NodeID`\s are not guaranteed to be equal between two `NodeManager`\s.

 - When persisting a `NodeStorage` special care must be taken that the `NodeStorage` is loaded next time with the
   same `NodeStorageID` as last time. Otherwise, the `NodeBackendHandle`\s will be invalid.