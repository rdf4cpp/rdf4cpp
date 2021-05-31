//
// Created by Lixi Alié Conrads on 5/29/21.
//

#ifndef RDF4CPP_ERR_H
#define RDF4CPP_ERR_H

#include <variant>
#include <optional>
#include "Result.h"

namespace rdf4cpp::utils::sec {

    ///
    /// An Error Result Wrapper which can be used to show that the Result is an Error.
    ///
    /// \tparam V The Ok Value (is only needed for correct casting)
    /// \tparam K The Error Value which this object should contain
    template<class V, class K>
    class Err : public  Result<V, K> {
    public:

        ///
        /// Creates the Error Result Object instantiating with the K value.
        ///
        /// \param value the value the error object should contain. For example a String message, or a throwable object
        explicit Err(K value){
            this->value=value;
        }
    };

}

#endif //RDF4CPP_ERR_H
