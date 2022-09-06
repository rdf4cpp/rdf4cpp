#ifndef RDF4CPP_DATATYPECONVERSIONTYPING_HPP
#define RDF4CPP_DATATYPECONVERSIONTYPING_HPP

#include <concepts>
#include <tuple>
#include <type_traits>
#include <variant>

#include <rdf4cpp/rdf/datatypes/registry/util/Tuple.hpp>
#include <rdf4cpp/rdf/datatypes/LiteralDatatype.hpp>
#include <rdf4cpp/rdf/datatypes/registry/DatatypeIRI.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

/**
 * A conversion from a source_type to a target_type
 */
template<typename T>
concept ConversionEntry = requires(typename T::source_type::cpp_type value) {
                              requires LiteralDatatype<typename T::source_type>;
                              requires LiteralDatatype<typename T::target_type>;
                              { T::convert(value) } -> std::same_as<typename T::target_type::cpp_type>;
                          };


namespace conversion_typing_detail {

template<typename T>
struct IsConversionLayer : std::false_type {};

template<typename... Ts>
struct IsConversionLayer<std::tuple<Ts...>>
    : std::bool_constant<
              (ConversionEntry<Ts> && ...) && util::AllSame<typename Ts::source_type...>::value> {};


template<typename Layer>
struct ConversionLayerSource;

template<typename Entry, typename... Entries>
struct ConversionLayerSource<std::tuple<Entry, Entries...>> {
    using type = typename Entry::source_type;
};

template<typename T>
struct IsConversionTable
    : std::false_type {};

template<typename... Ts>
struct IsConversionTable<std::tuple<Ts...>>
    : std::bool_constant<
              (IsConversionLayer<Ts>::value && ...) && util::AllSame<typename ConversionLayerSource<Ts>::type...>::value> {};

}  // namespace conversion_typing_detail


/**
 * A tuple of ConversionEntries
 * where all elements have the same associated ::source_type defined
 */
template<typename T>
concept ConversionLayer = conversion_typing_detail::IsConversionLayer<T>::value;

/**
 * A tuple of ConversionLayers
 * where all all ConversionEntries in all ConversionLayers have the same associated ::source_type defined.
 */
template<typename T>
concept ConversionTable = conversion_typing_detail::IsConversionTable<T>::value;

/**
 * A type erased version of a ConversionEntry.
 */
struct RuntimeConversionEntry {
    using convert_fptr_t = std::any (*)(std::any const &);

    DatatypeIRI target_type_iri;
    convert_fptr_t convert;

    template<ConversionEntry Entry>
    static RuntimeConversionEntry from_concrete() {
        DatatypeIRI target_type_iri = []() {
            if constexpr (FixedIdLiteralDatatype<typename Entry::target_type>) {
                return DatatypeIRI{Entry::target_type::fixed_id};
            } else {
                return DatatypeIRI{std::string{Entry::target_type::identifier}};
            }
        }();

        return RuntimeConversionEntry{
                .target_type_iri = std::move(target_type_iri),
                .convert = [](std::any const &value) -> std::any {
                    auto actual_value = std::any_cast<typename Entry::source_type::cpp_type>(value);
                    return Entry::convert(actual_value);
                }};
    }
};

/**
 * A runtime accessible version of a compile-time conversion table.
 */
struct RuntimeConversionTable {
private:
    size_t s_rank;
    size_t max_p_rank;
    std::vector<size_t> p_ranks;
    std::vector<RuntimeConversionEntry> table;

    RuntimeConversionTable(size_t const s_rank, size_t const max_p_rank)
        : s_rank{s_rank}, max_p_rank{max_p_rank}, p_ranks(s_rank)
    {
        if (s_rank * max_p_rank > 0) {
            table.resize(s_rank * max_p_rank, RuntimeConversionEntry{
                                                      .target_type_iri = DatatypeIRI{storage::node::identifier::LiteralType{}},
                                                      .convert = nullptr});
        }
    }

    inline static RuntimeConversionTable empty() {
        return RuntimeConversionTable{0, 0};
    }

    /**
     * mutable version of conversion_at_index
     * for more details see: const version of conversion_at_index
     */
    inline RuntimeConversionEntry &conversion_at_index(size_t s_off, size_t p_off) noexcept {
        assert(s_off < s_rank);
        assert(p_off < promotion_rank_at_level(s_off));

        return table[s_off * max_p_rank + p_off];
    }


    friend class DatatypeRegistry;

public:
    template<ConversionTable Table>
    static RuntimeConversionTable from_concrete() {
        static constexpr size_t s_rank = std::tuple_size_v<Table>;

        static constexpr size_t max_p_rank = util::tuple_type_fold<Table>(0ul, []<ConversionLayer Layer>(auto acc) {
            return std::max(acc, std::tuple_size_v<Layer>);
        });

        RuntimeConversionTable table{s_rank, max_p_rank};

        util::tuple_type_fold<Table>(0ul, [&]<ConversionLayer Layer>(size_t const s_off) {
            table.p_ranks[s_off] = std::tuple_size_v<Layer>;

            util::tuple_type_fold<Layer>(0ul, [&]<ConversionEntry Entry>(size_t const p_off) {
                table.conversion_at_index(s_off, p_off) = RuntimeConversionEntry::from_concrete<Entry>();
                return p_off + 1;
            });

            return s_off + 1;
        });

        return table;
    }

    /**
     * @return the type's subtype rank
     */
    [[nodiscard]] inline size_t subtype_rank() const noexcept {
        return s_rank;
    }

    /**
     * @param subtype_level how many subtype substitutions up to calculate the promotion rank
     * @return the type's promotion rank at a given subtype offset
     */
    [[nodiscard]] inline size_t promotion_rank_at_level(size_t const subtype_offset) const noexcept {
        return p_ranks[subtype_offset];
    }

    /**
     * Get the defined conversion for a given subtype offset and promotion offset.
     *
     * SAFETY: the user needs to ensure the following assertions hold, otherwise the behaviour is undefined
     *      - s_off < s_rank
     *      - p_off < promotion rank at subtype offset s_off
     *
     * @param s_off subtype offset, how many subtype substitutions up the conversion is
     * @param p_off promotion offset, how many promotions right given the subtype offset the conversion is
     * @return the found conversion
     */
    [[nodiscard]] inline RuntimeConversionEntry const &conversion_at_index(size_t s_off, size_t p_off) const noexcept {
        assert(s_off < s_rank);
        assert(p_off < promotion_rank_at_level(s_off));

        return table[s_off * max_p_rank + p_off];
    }
};

}  // namespace rdf4cpp::rdf::datatypes::registry

#endif  //RDF4CPP_DATATYPECONVERSIONTYPING_HPP
