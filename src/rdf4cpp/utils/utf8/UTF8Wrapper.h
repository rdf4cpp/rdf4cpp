//
// Created by Lixi Alié Conrads on 6/1/21.
//

#ifndef RDF4CPP_UTF8WRAPPER_H
#define RDF4CPP_UTF8WRAPPER_H

#include "../sec/Result.h"
#include "../error/InvalidUTF8StringError.h"

//TODO make UTF8Wrappers thread safe using mutexes
namespace rdf4cpp::utils::utf8 {

    ///
    /// Abstract UTF8 Wrapper class
    ///
    class UTF8Wrapper {
    public:
        /// Checks if next char is a valid utf 8 char (char is a utf8 char and not an 1 byte char)
        ///
        /// \return true if contains next char, false otherwise
        [[nodiscard]] virtual bool has_next() const = 0;

        /// An utf8 char which maybe 1, 2, 3 or 4 bytes long.
        ///
        /// \return the utf8 char (1,2,3 or 4 bytes) represented as a string
        virtual std::optional<std::string> next() = 0;


        static bool is_valid_utf8_char(std::string &utf8_char);

        virtual ~UTF8Wrapper();
    };
}

#endif //RDF4CPP_UTF8WRAPPER_H
