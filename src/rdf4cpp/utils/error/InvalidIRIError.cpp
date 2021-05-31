//
// Created by Lixi Alié Conrads on 5/31/21.
//

#include "InvalidIRIError.h"

#include <utility>

rdf4cpp::utils::error::InvalidIRIError::InvalidIRIError(std::string message) {
    this->message=std::move(message);
}

std::string rdf4cpp::utils::error::InvalidIRIError::get_message() const {
    return this->message;
}
