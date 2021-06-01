//
// Created by Lixi Alié Conrads on 6/1/21.
//

#include "UTF8Wrapper.h"

bool rdf4cpp::utils::utf8::UTF8Wrapper::is_valid_utf8_char(std::string utf8_char) {
    u_char init_char = utf8_char[0];
    if (init_char<0x80){ // 1 byte ascii char
        return utf8_char.length()==1;
    }
    else if (init_char&0xC0 && utf8_char.length()==2){ // 2byte utf 8 char
        return ((u_char)utf8_char[1])&0x80;
    }
    else if (init_char&0xE0 && utf8_char.length()==3){ // 3byte utf 8 char
        return ((u_char)utf8_char[1])&0x80  && ((u_char)utf8_char[2])&0x80;
    }
    else if (init_char&0xF0 && utf8_char.length()==4){ // 4byte utf 8 char
        return ((u_char)utf8_char[1])&0x80  && ((u_char)utf8_char[2])&0x80  && ((u_char)utf8_char[3])&0x80;
    }
    else{
        return false;
    }
}
