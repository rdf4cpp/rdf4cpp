//
// Created by kaimal on 07.12.21.
//

#ifndef RDF4CPP_XSD_DATE_HPP
#define RDF4CPP_XSD_DATE_HPP

#include <cstdint>
#include <ostream>
#include <rdf4cpp/rdf/datatypes/DatatypeRegistry.hpp>

namespace rdf4cpp::rdf::datatypes::xsd {
using Date = time_t;  //!< Implements <a href="http://www.w3.org/2001/XMLSchema#date">xsd:date</a>
}

namespace rdf4cpp::rdf::datatypes {
template<>
inline std::string RegisteredDatatype<xsd::Date>::datatype_iri() noexcept { return "http://www.w3.org/2001/XMLSchema#date"; }

template<>
inline xsd::Date RegisteredDatatype<xsd::Date>::from_string(const std::string &s) {
    return std::stol(s);
}
}  // namespace rdf4cpp::rdf::datatypes


#endif  //RDF4CPP_XSD_DATE_HPP
