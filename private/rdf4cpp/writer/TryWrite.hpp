#ifndef RDF4CPP_PRIVATE_TRY_SERIALIZE_HPP
#define RDF4CPP_PRIVATE_TRY_SERIALIZE_HPP

#include <rdf4cpp/writer/BufWriter.hpp>

#define RDF4CPP_DETAIL_TRY_WRITE_STR(...)                           \
    if (!::rdf4cpp::writer::write_str({__VA_ARGS__}, writer)) { \
        return false;                                               \
    }

#endif // RDF4CPP_PRIVATE_TRY_SERIALIZE_HPP
