//
// Created by Lixi Alié Conrads on 5/31/21.
//

#include "InvalidUTF8StringError.h"

rdf4cpp::utils::error::InvalidUTF8StringError::InvalidUTF8StringError(std::string message) {
    this->message=std::move(message);
}

std::string rdf4cpp::utils::error::InvalidUTF8StringError::get_message() const {
    return this->message;
}
