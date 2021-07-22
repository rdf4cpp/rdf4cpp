//
// Created by Lixi Alié Conrads on 6/1/21.
//

#include <fstream>
#include "UTF8StringStream.h"
#include "../sec/Ok.h"

rdf4cpp::utils::utf8::UTF8StringStream
rdf4cpp::utils::utf8::UTF8StringStream::create(std::ifstream utf8_stream) {
    return rdf4cpp::utils::utf8::UTF8StringStream(std::move(utf8_stream));
}

rdf4cpp::utils::utf8::UTF8StringStream::UTF8StringStream([[maybe_unused]]std::ifstream utf8_stream) {
    // TODO: implement
//    std::move(utf8_stream);
}

bool rdf4cpp::utils::utf8::UTF8StringStream::has_next() const {
    return false;
}

std::optional<std::string> rdf4cpp::utils::utf8::UTF8StringStream::next() {
    return std::string();
}
