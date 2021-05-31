//
// Created by Lixi Alié Conrads on 5/29/21.
//

#ifndef RDF4CPP_RDFNODE_H
#define RDF4CPP_RDFNODE_H


#include <optional>
#include <string>

namespace rdf4cpp::rdf::node{

    ///
    /// The abstract RDFNode class, containing either a Literal, Variable, IRIResource or BlankNode
    ///
    class RDFNode {
    public:

        virtual ~RDFNode(){};

        [[nodiscard]] virtual std::string as_string(bool quoting) const = 0;

        virtual void bla(){};

        ///
        /// \return
        [[nodiscard]] virtual bool is_literal() const{
            return false;
        }

        ///
        /// \return
        [[nodiscard]] virtual bool is_var() const {
            return false;
        };

        ///
        /// \return
        [[nodiscard]] virtual bool is_blank_node() const {
            return false;
        };

        ///
        /// \return
        [[nodiscard]] virtual bool is_iri() const {
            return false;
        };

    };
}



#endif //RDF4CPP_RDFNODE_H
