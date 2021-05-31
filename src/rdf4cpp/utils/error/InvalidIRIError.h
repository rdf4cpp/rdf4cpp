//
// Created by Lixi Alié Conrads on 5/31/21.
//

#ifndef RDF4CPP_INVALIDIRIERROR_H
#define RDF4CPP_INVALIDIRIERROR_H


#include <exception>
#include <string>

namespace rdf4cpp::utils::error {

    class InvalidIRIError : public std::exception {
    public:
        explicit InvalidIRIError(std::string message);
        InvalidIRIError()=default;

        [[nodiscard]] std::string get_message() const;

    private:
        std::string message;
    };

}

#endif //RDF4CPP_INVALIDIRIERROR_H
