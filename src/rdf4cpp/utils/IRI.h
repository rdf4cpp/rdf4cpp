//
// Created by Lixi Alié Conrads on 5/31/21.
//

#ifndef RDF4CPP_IRI_H
#define RDF4CPP_IRI_H

#include <string>
#include "sec/Result.h"
#include "error/InvalidIRIError.h"

namespace rdf4cpp::utils {

    ///
    /// IRI containing a valid IRI string.
    ///
    class IRI {
    public:

        //TODO read from stream constructor static sec::Result<IRI, error::InvalidIRIError> create(stream &stream, char end_char);

        /// Creates an IRI containing an IRI string, if the string is a valid IRI.
        /// If the iri_string isn't valid, it will return the error message why it's invalid.
        ///
        /// \param iri_string the IRI represented as a String
        /// \return a Result containing either an IRI if the iri_string is a valid IRI, the corresponding Error message if not
        static sec::Result<IRI, error::InvalidIRIError> create(std::string iri_string);

        /// Gets the IRI represented as a string
        ///
        /// \return a string representation of this IRI
        [[nodiscard]] std::string get_iri() const;

        /// Checks if a string is a valid IRI
        /// <br/><br/>
        /// shortcut for `rdf4cpp::utils::IRI::create(iri_string).is_ok()`
        ///
        /// \param iri_string String to validate
        /// \return true if iri_string is valid IRI, false otherwise
        static bool is_valid(std::string &iri_string) ;

        /// Checks if the IRI is also a valid URI (is only ASCII)
        /// \return true if this is also a valid URI, false otherwise
        [[nodiscard]] bool is_uri() const;

    private:
        explicit IRI(std::string iri_string);
        std::string iri;
    };

}
#endif //RDF4CPP_IRI_H
