//
// Created by Lixi Alié Conrads on 6/1/21.
//

#ifndef RDF4CPP_STRINGVALIDATOR_H
#define RDF4CPP_STRINGVALIDATOR_H

#include <string>
#include "sec/Result.h"
#include "utf8/UTF8Wrapper.h"

namespace rdf4cpp::utils {

    typedef bool (* string_validate)(std::string to_validate);

    /// Class to validate a UTF8 Wrapper up to a certain utf8 char.
    ///
    class StringValidator {
    public:

        /// Validates an UTF8Wrapper using the provided validation function for each utf8 char up to a utf8 char (end_char)
        /// and returns the string up to that point.
        ///
        /// \param utf8_stream the utf8 wrapper
        /// \param end_char utf8 char on which the validation should stop.
        /// \param validate1 function to determine if a char is valid.
        /// \return Either Ok with a validated string or Error with the error message included.
        static sec::Result<std::string, std::exception>validate_up_to(utf8::UTF8Wrapper &utf8_stream, std::string &end_char, string_validate validate1);
    };
}

#endif //RDF4CPP_STRINGVALIDATOR_H
