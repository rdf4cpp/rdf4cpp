//
// Created by Lixi Alié Conrads on 5/31/21.
//

#ifndef RDF4CPP_UTF8STRING_H
#define RDF4CPP_UTF8STRING_H

#include <string>
#include <iterator>
#include "sec/Result.h"
#include "error/InvalidUTF8StringError.h"

/// UTF8 String Wrapper.
///
/// That I really have to build an effing UTF 8 Wrapper is speaking volumes for this language
class UTF8String{
public:

    /// Checks if the given string is a valid utf8 string and return a result object containing the valid UTF8 String.
    ///
    /// \param utf8_string The string to convert
    /// \return Either a valid UTF8String object or an Error message containing the Error
    static rdf4cpp::utils::sec::Result<UTF8String, rdf4cpp::utils::error::InvalidUTF8StringError> create(std::string utf8_string);

    /// Checks if next char is a valid utf 8 char (char is a utf8 char and not an 1 byte char)
    ///
    /// \return true if contains next char, false otherwise
    bool has_next();

    /// An utf8 char which maybe 1, 2, 3 or 4 bytes long.
    ///
    /// \return the utf8 char (1,2,3 or 4 bytes) represented as a string
    std::string next();

private:
    size_t pos=0;
    std::string string;
    explicit UTF8String(std::string);
};


#endif //RDF4CPP_UTF8STRING_H
