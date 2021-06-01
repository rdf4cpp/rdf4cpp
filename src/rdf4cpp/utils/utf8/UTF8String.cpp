//
// Created by Lixi Alié Conrads on 5/31/21.
//

#include "UTF8String.h"
#include "../error/InvalidUTF8StringError.h"
#include "../sec/Result.h"
#include "../sec/Ok.h"
#include "../sec/Err.h"

rdf4cpp::utils::utf8::UTF8String::UTF8String(std::string string) {
    this->string = string;
}

bool rdf4cpp::utils::utf8::UTF8String::has_next() const {
    if( string.length()>pos ){
        u_char pos_char = string[pos];
        if (pos_char<0x80){ // 1 byte ascii char
            return true;
        }
        else if (pos_char<=0xDF && pos_char >= 0xC2){ // 2byte utf 8 char
            return string.length()>pos+1;
        }
        else if (pos_char<=0xEF && pos_char >= 0xE0){ // 3byte utf 8 char
            return string.length()>pos+2;
        }
        else if (pos_char<=0xF4 && pos_char >= 0xF0){ // 4byte utf 8 char
            return string.length()>pos+3;
        }
    }
    return false;
}

std::optional<std::string> rdf4cpp::utils::utf8::UTF8String::next() {
    u_char pos_char = string[pos];
    std::string ret;
    if (pos_char<0x80){ // 1 byte ascii char
        ret = string.substr(pos, pos+1);
        pos++;
    }
    else if (pos_char<=0xDF && pos_char >= 0xC2){ // 2byte utf 8 char
        ret = string.substr(pos, pos+2);
        pos+=2;
    }
    else if (pos_char<=0xEF && pos_char >= 0xE0){ // 3byte utf 8 char
        ret = string.substr(pos, pos+3);
        pos+=3;
    }
    else if (pos_char<=0xF4 && pos_char >= 0xF0){ // 4byte utf 8 char
        ret = string.substr(pos, pos+4);
        pos+=4;
    }
    else{
        return std::nullopt;
    }
    return std::optional<std::string>(ret);
}

rdf4cpp::utils::sec::Result<rdf4cpp::utils::utf8::UTF8String, rdf4cpp::utils::error::InvalidUTF8StringError> rdf4cpp::utils::utf8::UTF8String::create(std::string utf8_string) {
    u_int8_t size = 0;
    size_t pos =0;
    for(u_char c : utf8_string){
        if(size==0){ // new Char
            if (c<0x80){ // 1 byte ascii char
                //nothing to do here
            }
            else if (c<=0xDF && c >= 0xC2){ // 2byte utf 8 char
                size=1;
            }
            else if (c<=0xEF && c >= 0xE0){ // 3byte utf 8 char
                size=2;
            }
            else if (c<=0xF4 && c >= 0xF0){ // 4byte utf 8 char
                size=3;
            }else{
                // not valid
                return rdf4cpp::utils::sec::Err<UTF8String, rdf4cpp::utils::error::InvalidUTF8StringError>(
                        rdf4cpp::utils::error::InvalidUTF8StringError("Invalid UTF-8 Character at position "+pos)
                );
            }
        }
        else if(size>=1){ // 2 byte char, 2nd byte: 10xxxxxx
            if(!(c <= 0xBF && c >= 0x80)){
                // not valid
                return rdf4cpp::utils::sec::Err<UTF8String, rdf4cpp::utils::error::InvalidUTF8StringError>(
                        rdf4cpp::utils::error::InvalidUTF8StringError("Invalid UTF-8 Character at position "+pos)
                        );
            }
            size--;
        }
        pos++;

    }
    if (size!=0){
        //not valid
        return rdf4cpp::utils::sec::Err<UTF8String, rdf4cpp::utils::error::InvalidUTF8StringError>(
                rdf4cpp::utils::error::InvalidUTF8StringError("String was cut at end and is not valid UTF8.")
        );
    }
    // is valid utf-8 string.
    return rdf4cpp::utils::sec::Ok<UTF8String, rdf4cpp::utils::error::InvalidUTF8StringError>(UTF8String(std::move(utf8_string)));
}
