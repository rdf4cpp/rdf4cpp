//
// Created by Lixi Alié Conrads on 6/1/21.
//

#ifndef RDF4CPP_UTF8STRINGSTREAM_H
#define RDF4CPP_UTF8STRINGSTREAM_H

#include <string>
#include "../sec/Result.h"
#include "../error/InvalidUTF8StringError.h"
#include "UTF8Wrapper.h"

namespace rdf4cpp::utils::utf8 {

    class UTF8StringStream : public UTF8Wrapper{
    public:

        /// Creates a UTF8 String Wrapper around an ifstream.
        /// It is useful to use std::move(ifstream), as a reference will not be allowed due to security reasons.
        ///
        /// \param utf8_stream
        /// \return
        static UTF8StringStream create(std::ifstream utf8_stream);

        bool has_next() const override;

        std::optional<std::string> next() override;

    private:
        [[maybe_unused]] size_t pos = 0;
        std::ifstream utf8_stream{nullptr};

        explicit UTF8StringStream(std::ifstream utf8_stream);
    };
}
#endif //RDF4CPP_UTF8STRINGSTREAM_H
