#ifndef RDF4CPP_PRIVATE_TRY_SERIALIZE_HPP
#define RDF4CPP_PRIVATE_TRY_SERIALIZE_HPP

#include <rdf4cpp/rdf/writer/BufWriter.hpp>

#define RDF4CPP_DETAIL_TRY_WRITE_STR(...)                   \
    if (!write_str({__VA_ARGS__}, buffer, cursor, flush)) { \
        return false;                                       \
    }

#define RDF4CPP_DETAIL_TRY_SERIALIZE(obj)        \
    if (!obj.serialize(buffer, cursor, flush)) { \
        return false;                            \
    }

#endif // RDF4CPP_PRIVATE_TRY_SERIALIZE_HPP
