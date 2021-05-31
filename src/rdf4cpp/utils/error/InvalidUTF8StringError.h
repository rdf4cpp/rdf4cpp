//
// Created by Lixi Alié Conrads on 5/31/21.
//

#ifndef RDF4CPP_INVALIDUTF8STRINGERROR_H
#define RDF4CPP_INVALIDUTF8STRINGERROR_H

#include <string>

namespace rdf4cpp::utils::error {
    class InvalidUTF8StringError : public std::exception {
    public:
        explicit InvalidUTF8StringError(std::string message);

        InvalidUTF8StringError() = default;

        [[nodiscard]] std::string get_message() const;

    private:
        std::string message;
    };
}

#endif //RDF4CPP_INVALIDUTF8STRINGERROR_H
