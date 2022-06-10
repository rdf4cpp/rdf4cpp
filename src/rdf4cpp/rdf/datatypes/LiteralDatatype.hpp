#ifndef RDF4CPP_LITERALDATATYPE_HPP
#define RDF4CPP_LITERALDATATYPE_HPP
#include <rdf4cpp/rdf/datatypes/registry/ConstexprString.hpp>
namespace rdf4cpp::rdf::datatypes {
template<typename LiteralDatatypeImpl>
concept LiteralDatatype = requires(LiteralDatatypeImpl,
                                   std::string_view sv,
                                   typename LiteralDatatypeImpl::cpp_type const &cpp_value) {
                              typename LiteralDatatypeImpl::cpp_type;
                              { LiteralDatatypeImpl::identifier } -> std::convertible_to<std::string_view>;
                              { LiteralDatatypeImpl::identifier } -> std::convertible_to<std::string>;
                              { LiteralDatatypeImpl::from_string(sv) } -> std::convertible_to<typename LiteralDatatypeImpl::cpp_type>;
                              { LiteralDatatypeImpl::to_string(cpp_value) } -> std::convertible_to<std::string>;
                          };

}  // namespace rdf4cpp::rdf::datatypes
#endif  //RDF4CPP_LITERALDATATYPE_HPP
