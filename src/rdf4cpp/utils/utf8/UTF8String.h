//
// Created by Lixi Alié Conrads on 5/31/21.
//

#ifndef RDF4CPP_UTF8STRING_H
#define RDF4CPP_UTF8STRING_H

#include <string>
#include <iterator>
#include "../sec/Result.h"
#include "../error/InvalidUTF8StringError.h"
#include "UTF8Wrapper.h"

namespace rdf4cpp::utils::utf8 {

/// UTF8 String Wrapper Iterator.
///
/// That I really have to build an effing UTF 8 Wrapper is speaking volumes for this language
    class UTF8String : public UTF8Wrapper {
    public:


        /// Checks if the given string is a valid utf8 string and return a result object containing the valid UTF8 String.
        ///
        /// \param utf8_string The string to convert
        /// \return Either a valid UTF8String object or an Error message containing the Error
        static sec::Result<UTF8String, error::InvalidUTF8StringError>
        create(std::string utf8_string);


        [[nodiscard]] bool has_next() const override;

        std::optional<std::string> next() override;

    private:
        size_t pos = 0;
        std::string string;

        explicit UTF8String(std::string);
    };
}

#endif //RDF4CPP_UTF8STRING_H
