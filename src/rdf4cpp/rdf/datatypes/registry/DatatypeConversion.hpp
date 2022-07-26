#ifndef RDF4CPP_DATATYPECONVERSION_HPP
#define RDF4CPP_DATATYPECONVERSION_HPP

#include <any>
#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

#include <rdf4cpp/rdf/datatypes/LiteralDatatype.hpp>
#include <rdf4cpp/rdf/datatypes/registry/Util.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

struct RuntimeConversionEntry {
    std::string target_type_iri;
    std::function<std::any(std::any const &)> conversion_fn;
};

using RuntimeConversionTable = std::vector<std::vector<RuntimeConversionEntry>>;


namespace conversion_detail {

/**
 * function composition of two functions f and g
 * @return the composed function
 */
template<typename F, typename G>
constexpr auto compose_fns(F f, G g) {
    return [f, g](auto &&x) {
        return f(g(std::forward<decltype(x)>(x)));
    };
}

/**
 * A conversion to a TargetType
 *
 * @tparam TargetTypeImpl the target of the conversion
 * @tparam F the function to take an here unspecified BaseType to the TargetType
 *
 * Note: The base type is not specified here because this is only supposed
 *      to be used in conjunction with ConversionTable
 */
template<LiteralDatatype TargetTypeImpl, typename F>
struct ConversionEntry {
    using TargetType = TargetTypeImpl;
    F convert;
};

/**
 * A holder for a compiletime generated conversion table
 * of the form
 *
 * std::tuple<
 *     std::tuple<ConversionEntry<LiteralDatatype Target, ConversionFunction>, ...>,     <-- direct promotions of LiteralDatatypeImpl
 *     std::tuple<ConversionEntry<LiteralDatatype Target, ConversionFunction2>, ...>,    <-- direct promotions of Supertype of LiteralDatatypeImpl
 *     ...>
 *
 * @tparam LiteralDatatypeImpl The LiteralDatatype to which this table belongs
 * @tparam Table The actual compile time generated table type
 */
template<LiteralDatatype LiteralDatatypeImpl, typename Table>
struct ConversionTable {
    Table conversion_fns;

    /**
     * Turn the compile time representation of this conversion table (Table)
     * into a type erased representation that can be queried and used at runtime
     *
     * @return runtime representation of this
     */
    [[nodiscard]]
    inline RuntimeConversionTable into_runtime_table() const noexcept {
        return util::tuple_fold(this->conversion_fns, std::vector<std::vector<RuntimeConversionEntry>>{}, [](auto acc, auto const &layer) {
            auto rt = util::tuple_fold(layer, std::vector<RuntimeConversionEntry>{}, [](auto acc, auto const &ct_conv) {
                auto rte = RuntimeConversionEntry {
                        std::string{std::remove_cvref_t<decltype(ct_conv)>::TargetType::identifier},
                        [f = ct_conv.convert](std::any const &value) -> std::any {
                            auto cpp_value = std::any_cast<typename LiteralDatatypeImpl::cpp_type>(value);
                            return f(cpp_value);
                        }};

                acc.push_back(rte);
                return acc;
            });

            acc.push_back(rt);
            return acc;
        });
    }
};

/**
 * Generate a linear conversion table_acc
 * consisting only of conversions in one direction,
 * so either only promotion or only subtype substitution
 *
 * @tparam BaseType the type for which to generate the linear conversions
 * @tparam Type the current type getting processed in the linear hierarchy
 * @tparam Conversion a common interface to carry out a conversion (see adaptor namespace)
 * @tparam HasConversion check if a LiteralDatatype has a next conversion, e.g. if it is promotable
 * @tparam RankRule a boolean predicate to check if the defined conversion is valid
 * @tparam ConvertEntries types of the already existing ConversionEntries
 * @param table_acc accumulator for ConversionEntries
 * @return a std::tuple<ConversionEntry, ...> containing all linear conversions reachable from BaseType
 */
template<LiteralDatatype BaseType,
         LiteralDatatype Type,
         template<LiteralDatatype> typename Conversion,
         template<LiteralDatatype> typename HasConversion,
         template<LiteralDatatype, LiteralDatatype> typename RankRule,
         typename ...ConvertEntries>
consteval auto make_conversion_table_impl(std::tuple<ConvertEntries...> const &table_acc) {
    if constexpr (!HasConversion<Type>::value) {
        return table_acc;
    } else {
        using next = Conversion<Type>;

        // defining a hierarchy which does not follow the rank rules (see adaptors::PromoteRankRule, adaptors::SubtypeRankRule)
        // would result in not being able to find all conversions between types
        // and thus yield unexpected behaviour when trying to convert
        // e.g. with
        //        A --> Z               Legend:
        //        |     |                   --> : promotable to
        //        B --> Y --> X (!)          |  : subtype of
        //
        // The algorithm would not be able to find B --> X or Y --> X,
        // because X has subtype rank 0 while Y and B have subtype rank 1.
        // This is problematic when trying to jump to the common subtype level for both arguments
        // because while in the graph they are visually at the correct level, their subtype ranks do not reflect that.
        // The same problem exists when a supertype (or promoted type) has a subtype (promotion) rank that is not exactly one less than the origin type's rank.
        static_assert(RankRule<Type, typename next::converted>::value,
                      "detected invalid hierarchy, would not be able to discover all conversions");

        if constexpr (BaseType::identifier == Type::identifier) {
            ConversionEntry<typename next::converted, decltype(&next::convert)> next_conversion{&next::convert};

            auto next_table = std::tuple_cat(table_acc, std::make_tuple(next_conversion));
            return make_conversion_table_impl<BaseType, typename next::converted, Conversion, HasConversion, RankRule>(next_table);
        } else {
            using table_type_t = std::remove_reference_t<decltype(table_acc)>;
            auto const table_size = std::tuple_size_v<table_type_t>;

            auto const prev_promotion_fn = std::get<table_size - 1>(table_acc).convert;
            auto new_promotion_fn = compose_fns(next::convert, prev_promotion_fn);

            ConversionEntry<typename next::converted, decltype(new_promotion_fn)> next_conversion{new_promotion_fn};

            auto next_table = std::tuple_cat(table_acc, std::make_tuple(next_conversion));
            return make_conversion_table_impl<BaseType, typename next::converted, Conversion, HasConversion, RankRule>(next_table);
        }
    }
}

namespace adaptor {

/**
 * Adapt Promotion to a common "conversion" interface.
 */
template<LiteralDatatype LiteralDatatypeImpl>
struct PromoteConversion;

template<PromotableLiteralDatatype LiteralDatatypeImpl>
struct PromoteConversion<LiteralDatatypeImpl> {
    using cpp_type = typename LiteralDatatypeImpl::cpp_type;

    using converted = typename LiteralDatatypeImpl::promoted;
    using converted_cpp_type = typename LiteralDatatypeImpl::promoted_cpp_type;

    static constexpr size_t rank = LiteralDatatypeImpl::promotion_rank;

    inline static converted_cpp_type convert(cpp_type const &value) noexcept {
        return LiteralDatatypeImpl::promote(value);
    }
};

/**
 * Adapt subtype substitution to a common "conversion" interface
 */
template<LiteralDatatype LiteralDatatypeImpl>
struct SupertypeConversion;

template<SubtypedLiteralDatatype LiteralDatatypeImpl>
struct SupertypeConversion<LiteralDatatypeImpl> {
    using cpp_type = typename LiteralDatatypeImpl::cpp_type;

    using converted = typename LiteralDatatypeImpl::supertype;
    using converted_cpp_type = typename LiteralDatatypeImpl::super_cpp_type;

    static constexpr size_t rank = LiteralDatatypeImpl::subtype_rank;

    inline static converted_cpp_type convert(cpp_type const &value) noexcept {
        return LiteralDatatypeImpl::into_supertype(value);
    }
};

template<LiteralDatatype Type>
struct PromoteConcept : std::bool_constant<PromotableLiteralDatatype<Type>> {};

template<LiteralDatatype Type>
struct SubtypeConcept : std::bool_constant<SubtypedLiteralDatatype<Type>> {};

/**
 * - A promotion must preserve subtype rank
 * - A promotion must lower the promotion rank by exactly 1
 */
template<LiteralDatatype Type, LiteralDatatype Promoted>
struct PromoteRankRule : std::bool_constant<detail_rank::DatatypeSubtypeRank<Type::identifier>::value
                                                    == detail_rank::DatatypeSubtypeRank<Promoted::identifier>::value
                                            && detail_rank::DatatypePromotionRank<Promoted::identifier>::value
                                                       == detail_rank::DatatypePromotionRank<Type::identifier>::value - 1> {};

/**
 * - A subtype substitution must lower the subtype rank by exactly 1
 */
template<LiteralDatatype Type, LiteralDatatype Super>
struct SubtypeRankRule : std::bool_constant<detail_rank::DatatypeSubtypeRank<Super::identifier>::value
                                                    == detail_rank::DatatypeSubtypeRank<Type::identifier>::value - 1> {};

} // namespace adaptor

/**
 * Generate the linear conversion table consisting of type promotion conversions for a LiteralDatatype
 *
 * @tparam Type the datatype to generate the promotion table for
 * @return compiletime linear promotion table
 */
template<LiteralDatatype Type>
consteval auto make_promotion_table() {
    if constexpr (PromotableLiteralDatatype<Type>) {
        return conversion_detail::make_conversion_table_impl<Type,
                                                             Type,
                                                             conversion_detail::adaptor::PromoteConversion,
                                                             conversion_detail::adaptor::PromoteConcept,
                                                             conversion_detail::adaptor::PromoteRankRule>(std::tuple{});
    } else {
        return std::tuple{};
    }
}

/**
 * Generate the linear conversion table consisting of subtype substitution conversions for a LiteralDatatype
 *
 * @tparam Type the datatype to generate the subtype substitution table for
 * @return compiletime linear subtype substitution table
 */
template<LiteralDatatype Type>
consteval auto make_subtype_table() {
    if constexpr (SubtypedLiteralDatatype<Type>) {
        return conversion_detail::make_conversion_table_impl<Type,
                                                             Type,
                                                             conversion_detail::adaptor::SupertypeConversion,
                                                             conversion_detail::adaptor::SubtypeConcept,
                                                             conversion_detail::adaptor::SubtypeRankRule>(std::tuple{});
    } else {
        return std::tuple{};
    }
}

/**
 * Generate the combined table that contains conversions
 * for every type that is reachable from Type by a combination
 * of subtype substitution and type promotion.
 *
 * For a definition of reachable see make_conversion_table_for.
 *
 * @tparam Type the datatype to generate the combined table for
 * @return the combined table
 */
template<LiteralDatatype Type>
consteval auto make_combined_conversion_table() {
    auto id_f = [](auto &&x) { return std::forward<decltype(x)>(x); };
    conversion_detail::ConversionEntry<Type, decltype(id_f)> id_conversion{ id_f };

    auto level_0_table = std::tuple_cat(std::make_tuple(id_conversion), make_promotion_table<Type>());

    if constexpr (!SubtypedLiteralDatatype<Type>) {
        return std::make_tuple(level_0_table);
    } else {
        auto s_table = make_subtype_table<Type>();

        // generate the linear promotion table for each supertype
        auto other_level_tables = util::tuple_map(s_table, []<LiteralDatatype SuperType, typename SF>(ConversionEntry<SuperType, SF> const &to_super) {

            auto const level_p_table = make_promotion_table<SuperType>();

            // compose each promotion for the supertype with the function to convert to the supertype
            // to get direct conversions from Type to promoted supertype
            auto const to_promoted_supers = util::tuple_map(level_p_table, [=]<LiteralDatatype SuperPromotedType, typename PF>(ConversionEntry<SuperPromotedType, PF> const &promote_super) {

                auto const convert = conversion_detail::compose_fns(promote_super.convert, to_super.convert);
                return conversion_detail::ConversionEntry<SuperPromotedType, decltype(convert)>{convert};
            });

            return std::tuple_cat(std::make_tuple(to_super), to_promoted_supers);
        });

        return std::tuple_cat(std::make_tuple(level_0_table), other_level_tables);
    }
}

} // namespace conversion_detail


/**
 * a type that can be turned into a runtime conversion table
 */
template<typename Table>
concept IntoRuntimeConversionTable = requires(Table ctable) {
                                         { ctable.into_runtime_table() } -> std::convertible_to<RuntimeConversionTable>;
                                     };

/**
 * Generate a compiletime conversion table for the given type
 * this will include all conversions that are reachable from T.
 *
 * Reachable in this context means that there is a (*) directed path in the hierarchy
 * from T to the other type.
 *
 * *: only paths of the form
 *      supertype -> ... -> supertype -> promotion -> ... -> promotion
 *      are included, meaning paths that do a promotion first and then go to a supertype
 *      are not included
 *
 * @tparam T the type for which to generate the table
 * @return the generated table
 */
template<LiteralDatatype T>
consteval IntoRuntimeConversionTable auto make_conversion_table_for() {
    constexpr auto combined_table = conversion_detail::make_combined_conversion_table<T>();
    return conversion_detail::ConversionTable<T, decltype(combined_table)>{ combined_table };
}


} // namespace rdf4cpp::rdf::datatypes::registry

#endif  //RDF4CPP_DATATYPECONVERSION_HPP
