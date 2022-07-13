#ifndef RDF4CPP_LITERALDATATYPE_HPP
#define RDF4CPP_LITERALDATATYPE_HPP

#include <rdf4cpp/rdf/datatypes/registry/ConstexprString.hpp>

namespace rdf4cpp::rdf::datatypes {

template<typename LiteralDatatypeImpl>
concept LiteralDatatype = requires (LiteralDatatypeImpl, std::string_view sv, typename LiteralDatatypeImpl::cpp_type const &cpp_value) {
                                   typename LiteralDatatypeImpl::cpp_type;
                                   { LiteralDatatypeImpl::identifier } -> std::convertible_to<std::string_view>;
                                   { LiteralDatatypeImpl::from_string(sv) } -> std::convertible_to<typename LiteralDatatypeImpl::cpp_type>;
                                   { LiteralDatatypeImpl::to_string(cpp_value) } -> std::convertible_to<std::string>;
                               };

template<typename LiteralDatatypeImpl>
concept NumericLiteralDatatype = LiteralDatatype<LiteralDatatypeImpl>
                                 && requires (typename LiteralDatatypeImpl::cpp_type const &lhs, typename LiteralDatatypeImpl::cpp_type const &rhs) {
                                        { LiteralDatatypeImpl::add(lhs, rhs) } -> std::convertible_to<typename LiteralDatatypeImpl::cpp_type>;
                                        { LiteralDatatypeImpl::sub(lhs, rhs) } -> std::convertible_to<typename LiteralDatatypeImpl::cpp_type>;
                                        { LiteralDatatypeImpl::mul(lhs, rhs) } -> std::convertible_to<typename LiteralDatatypeImpl::cpp_type>;
                                        { LiteralDatatypeImpl::div(lhs, rhs) } -> std::convertible_to<typename LiteralDatatypeImpl::cpp_type>;
                                        { LiteralDatatypeImpl::pos(lhs) } -> std::convertible_to<typename LiteralDatatypeImpl::cpp_type>;
                                        { LiteralDatatypeImpl::neg(lhs) } -> std::convertible_to<typename LiteralDatatypeImpl::cpp_type>;
                                    };

template<typename LiteralDatatypeImpl>
concept LogicalLiteralDatatype = LiteralDatatype<LiteralDatatypeImpl>
                                 && requires (typename LiteralDatatypeImpl::cpp_type const &value) {
                                        { LiteralDatatypeImpl::effective_boolean_value(value) } -> std::convertible_to<bool>;
                                    };

template<typename LiteralDatatypeImpl>
concept PromotableLiteralDatatype = LiteralDatatype<LiteralDatatypeImpl>
                                    && requires (typename LiteralDatatypeImpl::cpp_type const &value) {
                                           typename LiteralDatatypeImpl::promoted;
                                           typename LiteralDatatypeImpl::promoted_cpp_type;
                                           { LiteralDatatypeImpl::promotion_rank } -> std::convertible_to<unsigned>;
                                           { LiteralDatatypeImpl::promote(value) } -> std::convertible_to<typename LiteralDatatypeImpl::promoted_cpp_type>;
                                       };

template<typename LiteralDatatypeImpl>
concept SubtypedLiteralDatatype = LiteralDatatype<LiteralDatatypeImpl>
                                  && requires (typename LiteralDatatypeImpl::cpp_type const &value) {
                                         typename LiteralDatatypeImpl::supertype;
                                         typename LiteralDatatypeImpl::super_cpp_type;
                                         { LiteralDatatypeImpl::subtype_rank } -> std::convertible_to<unsigned>;
                                         { LiteralDatatypeImpl::into_supertype(value) } -> std::convertible_to<typename LiteralDatatypeImpl::super_cpp_type>;
                                     };

}  // namespace rdf4cpp::rdf::datatypes
#endif  //RDF4CPP_LITERALDATATYPE_HPP
