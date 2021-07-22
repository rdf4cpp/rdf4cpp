//
// Created by Lixi Alié Conrads on 6/1/21.
//

#include "StringValidator.h"
#include "sec/Err.h"

rdf4cpp::utils::sec::Result<std::string, std::exception>
rdf4cpp::utils::StringValidator::validate_up_to([[maybe_unused]] rdf4cpp::utils::utf8::UTF8Wrapper &utf8_stream, std::string &end_char,
                                                rdf4cpp::utils::string_validate validate1) {
    //check if end_char is valid utf8 Char
    if (!utf8::UTF8Wrapper::is_valid_utf8_char(end_char)) {
        return sec::Err<std::string, std::exception>(error::InvalidUTF8StringError("end_char is not a valid UTF8 char."));
    }
    //todo while next until end_char is found or non valid char
    if (validate1("abc")) {
    }
    // TODO: unimplemented
    return sec::Err<std::string, std::exception>(error::InvalidUTF8StringError("Not yet implemented."));
}
