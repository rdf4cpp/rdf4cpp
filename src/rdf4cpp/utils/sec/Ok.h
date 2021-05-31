//
// Created by Lixi Alié Conrads on 5/29/21.
//

#ifndef RDF4CPP_OK_H
#define RDF4CPP_OK_H


#include <variant>
#include <optional>
#include "Result.h"

namespace rdf4cpp::utils::sec {


    ///
    /// An Ok Result Wrapper which can be used to show that the Result is valid and ok.
    ///
    /// \tparam V The Ok Value which this object should contain
    /// \tparam K The Error Value (is only needed for correct casting)
    template<class V, class K> class Ok : public Result<V, K> {
    public:

        ///
        /// Creates the Ok Result Object instantiating with the V value.
        ///
        /// \param value the value the Ok object should contain.
        explicit Ok(V value){
            this->value=value;
        }
    };
}



#endif //RDF4CPP_OK_H
