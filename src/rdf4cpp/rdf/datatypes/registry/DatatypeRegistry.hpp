#ifndef RDF4CPP_DATATYPEREGISTRY_HPP
#define RDF4CPP_DATATYPEREGISTRY_HPP

#include <rdf4cpp/rdf/datatypes/LiteralDatatype.hpp>
#include <rdf4cpp/rdf/datatypes/registry/DatatypeConversion.hpp>

#include <algorithm>
#include <any>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace rdf4cpp::rdf::datatypes::registry {

/**
 * Registry for LiteralDatatype implementations.
 * Data types are registered by defining, implementing and specializing members of LiteralDatatype.
 * @see LiteralDatatype
 */
class DatatypeRegistry {
public:
    /**
     * Constructs an instance of a type from a string.
     */
    using factory_fptr_t = std::any (*)(std::string_view);
    using to_string_fptr_t = std::string (*)(const std::any &);
    using ebv_fptr_t = bool (*)(std::any const &);

    struct NumericOpResult {
        std::string_view result_type_iri;
        std::any result_value;
    };

    using unop_fptr_t = NumericOpResult (*)(std::any const &);
    using binop_fptr_t = NumericOpResult (*)(std::any const &, std::any const &);

    struct NumericOps {
        binop_fptr_t add_fptr;  // a + b
        binop_fptr_t sub_fptr;  // a - b
        binop_fptr_t mul_fptr;  // a * b
        binop_fptr_t div_fptr;  // a / b

        unop_fptr_t pos_fptr;  // +a
        unop_fptr_t neg_fptr;  // -a
    };

    struct DatatypeEntry {
        std::string datatype_iri;         // datatype IRI string
        factory_fptr_t factory_fptr;      // construct from string
        to_string_fptr_t to_string_fptr;  // convert to string

        ebv_fptr_t ebv_fptr;  // convert to effective boolean value

        std::optional<NumericOps> numeric_ops;

        RuntimeConversionTable conversion_table;

        inline static DatatypeEntry for_search(std::string_view const datatype_iri) {
            return DatatypeEntry{
                    .datatype_iri = std::string{datatype_iri},
                    .factory_fptr = nullptr,
                    .to_string_fptr = nullptr,
                    .ebv_fptr = nullptr,
                    .numeric_ops = std::nullopt,
                    .conversion_table = RuntimeConversionTable::empty()};
        }
    };

    struct DatatypeConverter {
        std::string_view target_type_iri;
        RuntimeConversionEntry::convert_fptr_t convert_lhs;
        RuntimeConversionEntry::convert_fptr_t convert_rhs;

        inline static DatatypeConverter from_individuals(RuntimeConversionEntry const &l, RuntimeConversionEntry const &r) noexcept {
            assert(l.target_type_iri == r.target_type_iri);

            return DatatypeConverter{
                    .target_type_iri = l.target_type_iri,
                    .convert_lhs = l.convert,
                    .convert_rhs = r.convert};
        }
    };

private:
    using registered_datatypes_t = std::vector<DatatypeEntry>;

    inline static registered_datatypes_t &get_mutable() {
        static registered_datatypes_t registry_;
        return registry_;
    }

    /**
     * Tries to find the datatype corresponding to datatype_iri
     * if found `f` is applied to it to form the function result.
     *
     * @param datatype_iri the datatype to search
     * @param f function that maps a DatatypeEntry to (typically) a member of it
     * @return the result of `f` applied to the DatatypeEntry, if it exists, else nullopt
     */
    template<typename Map>
        requires std::invocable<Map, DatatypeEntry const &>
    static std::optional<std::invoke_result_t<Map, DatatypeEntry const &>> find_map_entry(std::string_view const datatype_iri, Map f) {
        auto const &registry = registered_datatypes();
        auto found = std::lower_bound(registry.begin(), registry.end(),
                                      DatatypeEntry::for_search(datatype_iri),
                                      [](const auto &left, const auto &right) { return left.datatype_iri < right.datatype_iri; });

        if (found != registry.end() and found->datatype_iri == datatype_iri) {
            return f(*found);
        } else {
            return std::nullopt;
        }
    }

    /**
     * Creates NumericOps based on a NumericLiteralDatatype
     * by generating type-erased versions of all necessary functions (add, sub, ...).
     */
    template<datatypes::NumericLiteralDatatype datatype_info>
    inline static NumericOps make_numeric_ops();

public:
    /**
     * Auto-register a datatype that fulfills DatatypeConcept
     * @tparam datatype_info type that is registered.
     */
    template<datatypes::LiteralDatatype datatype_info>
    inline static void add();

    /**
     * Register an datatype manually
     * @param datatype_iri datatypes iri
     * @param factory_fptr factory function to construct an instance from a string
     * @param to_string_fptr converts type instance to its string representation
     */
    inline static void add(DatatypeEntry entry_to_add) {
        auto &registry = DatatypeRegistry::get_mutable();
        auto found = std::find_if(registry.begin(), registry.end(), [&](const auto &entry) { return entry.datatype_iri == entry_to_add.datatype_iri; });
        if (found == registry.end()) {
            registry.push_back(entry_to_add);

            std::sort(registry.begin(), registry.end(),
                      [](const auto &left, const auto &right) { return left.datatype_iri < right.datatype_iri; });
        } else {
            *found = entry_to_add;
        }
    }

    /**
     * Retrieve all registered datatypes.
     * @return vector of pairs mapping datatype IRI std::string to factory_fptr_t
     */
    inline static const registered_datatypes_t &registered_datatypes() {
        return DatatypeRegistry::get_mutable();
    }

    /**
     * Get the database entry for a datatype_iri.
     * @param datatype_iri datatype IRI string
     * @return if available database entry else nullopt
     */
    inline static DatatypeEntry const *get_entry(std::string_view const datatype_iri) {
        auto const res = find_map_entry(datatype_iri, [](auto const &entry) {
            return &entry;
        });

        return res.has_value() ? *res : nullptr;
    }

    /**
     * Get a factory_fptr_t for a datatype IRI std::string. The factory_fptr_t can be used like `std::any type_instance = factory_fptr("types string repressentation")`.
     * @param datatype_iri datatype IRI std::string
     * @return function pointer or nullptr
     */
    inline static factory_fptr_t get_factory(std::string_view datatype_iri) {
        auto const res = find_map_entry(datatype_iri, [](auto const &entry) {
            return entry.factory_fptr;
        });

        return res.has_value() ? *res : nullptr;
    }

    /**
     * Get a to_string function for a datatype IRI std::string. The factory_fptr_t can be used like `std::string str_repr = to_string_fptr(any_typed_arg)`.
     * @param datatype_iri datatype IRI std::string
     * @return function pointer or nullptr
     */
    inline static to_string_fptr_t get_to_string(std::string_view datatype_iri) {
        auto const res = find_map_entry(datatype_iri, [](auto const &entry) {
            return entry.to_string_fptr;
        });

        return res.has_value() ? *res : nullptr;
    }

    /**
     * Try to get the numerical ops function table for a datatype IRI.
     * Returns nullopt if the datatype is not numeric, or does not exist.
     *
     * @param datatype_iri datatype IRI string
     * @return if available a structure containing function pointers for all numeric ops
     */
    inline static std::optional<NumericOps> get_numerical_ops(std::string_view const datatype_iri) {
        auto const res = find_map_entry(datatype_iri, [](auto const &entry) {
            return entry.numeric_ops;
        });

        return res.has_value() ? *res : std::nullopt;
    }

    /**
     * Try to get the logical ops function table for a datatype IRI.
     * Returns nullptr if the datatype is not logical, or does not exist.
     *
     * @param datatype_iri datatype IRI string
     * @return if available a structure containing function pointers for all logical ops
     */
    inline static ebv_fptr_t get_ebv(std::string_view const datatype_iri) {
        auto const res = find_map_entry(datatype_iri, [](auto const &entry) {
            return entry.ebv_fptr;
        });

        return res.has_value() ? *res : nullptr;
    }

    /**
     * Tries to find a conversion to convert lhs_type_iri and rhs_type_iri into a
     * common type to be used in e.g. numeric calculations.
     *
     * @return if possible a conversion that converts lhs and rhs into a common type
     */
    inline static std::optional<DatatypeConverter> get_common_type_conversion(std::string_view lhs_type_iri, std::string_view rhs_type_iri);
};


/**
 * To register a datatype, at least a LiteralDatatypeImpl instantiation must be provided.
 * If LiteralDatatype_t::cpp_type does not overload `operator<<`, the to_string(const datatype_t &value) must be specialized.
 * @tparam LiteralDatatype_t datatype that is being registered
 */
template<datatypes::LiteralDatatype LiteralDatatype_t>
inline void DatatypeRegistry::add() {
    auto const num_ops = []() -> std::optional<NumericOps> {
        if constexpr (datatypes::NumericLiteralDatatype<LiteralDatatype_t>) {
            return make_numeric_ops<LiteralDatatype_t>();
        } else {
            return std::nullopt;
        }
    }();

    auto const ebv_fptr = []() -> ebv_fptr_t {
        if constexpr (datatypes::LogicalLiteralDatatype<LiteralDatatype_t>) {
            return [](std::any const &operand) -> bool {
                auto const &operand_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(operand);
                return LiteralDatatype_t::effective_boolean_value(operand_val);
            };
        } else {
            return nullptr;
        }
    }();

    DatatypeRegistry::add(DatatypeEntry{
            .datatype_iri = std::string{LiteralDatatype_t::identifier},
            .factory_fptr = [](std::string_view string_repr) -> std::any {
                return std::any(LiteralDatatype_t::from_string(string_repr));
            },
            .to_string_fptr = [](const std::any &value) -> std::string {
                return LiteralDatatype_t::to_string(std::any_cast<typename LiteralDatatype_t::cpp_type>(value));
            },
            .ebv_fptr = ebv_fptr,
            .numeric_ops = num_ops,
            .conversion_table = make_runtime_conversion_table_for<LiteralDatatype_t>()});
}

namespace detail {

template<typename OpRes, LiteralDatatype Fallback>
struct SelectOpRes {
    using type = OpRes;
};

template<LiteralDatatype Fallback>
struct SelectOpRes<std::false_type, Fallback> {
    using type = Fallback;
};

} // namespace detail

template<datatypes::NumericLiteralDatatype LiteralDatatype_t>
inline DatatypeRegistry::NumericOps DatatypeRegistry::make_numeric_ops() {
    return NumericOps{
            // a + b
            [](std::any const &lhs, std::any const &rhs) -> NumericOpResult {
                auto const &lhs_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(lhs);
                auto const &rhs_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(rhs);

                return NumericOpResult {
                        .result_type_iri = detail::SelectOpRes<typename LiteralDatatype_t::add_result, LiteralDatatype_t>::type::identifier,
                        .result_value = LiteralDatatype_t::add(lhs_val, rhs_val)};
            },
            // a - b
            [](std::any const &lhs, std::any const &rhs) -> NumericOpResult {
                auto const &lhs_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(lhs);
                auto const &rhs_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(rhs);

                return NumericOpResult{
                        .result_type_iri = detail::SelectOpRes<typename LiteralDatatype_t::sub_result, LiteralDatatype_t>::type::identifier,
                        .result_value = LiteralDatatype_t::sub(lhs_val, rhs_val)};
            },
            // a * b
            [](std::any const &lhs, std::any const &rhs) -> NumericOpResult {
                auto const &lhs_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(lhs);
                auto const &rhs_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(rhs);

                return NumericOpResult{
                        .result_type_iri = detail::SelectOpRes<typename LiteralDatatype_t::mul_result, LiteralDatatype_t>::type::identifier,
                        .result_value = LiteralDatatype_t::mul(lhs_val, rhs_val)};
            },
            // a / b
            [](std::any const &lhs, std::any const &rhs) -> NumericOpResult {
                auto const &lhs_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(lhs);
                auto const &rhs_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(rhs);

                return NumericOpResult{
                        .result_type_iri = detail::SelectOpRes<typename LiteralDatatype_t::div_result, LiteralDatatype_t>::type::identifier,
                        .result_value = LiteralDatatype_t::div(lhs_val, rhs_val)};
            },
            // +a
            [](std::any const &operand) -> NumericOpResult {
                auto const &operand_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(operand);

                return NumericOpResult{
                        .result_type_iri = detail::SelectOpRes<typename LiteralDatatype_t::pos_result, LiteralDatatype_t>::type::identifier,
                        .result_value = LiteralDatatype_t::pos(operand_val)};
            },
            // -a
            [](std::any const &operand) -> NumericOpResult {
                auto const &operand_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(operand);

                return NumericOpResult{
                        .result_type_iri = detail::SelectOpRes<typename LiteralDatatype_t::neg_result, LiteralDatatype_t>::type::identifier,
                        .result_value = LiteralDatatype_t::neg(operand_val)};
            }
    };
}

inline std::optional<DatatypeRegistry::DatatypeConverter> DatatypeRegistry::get_common_type_conversion(
        std::string_view const lhs_type_iri,
        std::string_view const rhs_type_iri) {

    auto const find_conv_impl = [](RuntimeConversionTable const &lesser, RuntimeConversionTable const &greater) -> std::optional<DatatypeConverter> {
        auto const lesser_s_rank = lesser.subtype_rank();
        auto const greater_s_rank = greater.subtype_rank();

        // lesser should be conversiont table of the type with lower subtype rank
        assert(lesser_s_rank <= greater_s_rank);

        // calculate initial subtype offsets to equalize subtype rank
        size_t lesser_s_off = 0;
        size_t greater_s_off = greater_s_rank - lesser_s_rank;

        while (lesser_s_off < lesser_s_rank && greater_s_off < greater_s_rank) {
            auto const lesser_p_rank = lesser.promotion_rank_at_level(lesser_s_off);
            auto const greater_p_rank = greater.promotion_rank_at_level(greater_s_off);

            if (lesser_p_rank == greater_p_rank) {
                // subtype rank and promotion rank equal
                // => potential for correct conversion

                RuntimeConversionEntry const &lconv = lesser.conversion_at_index(lesser_s_off, 0);
                RuntimeConversionEntry const &gconv = greater.conversion_at_index(greater_s_off, 0);

                if (lconv.target_type_iri == gconv.target_type_iri) {
                    // correct conversion found
                    return DatatypeConverter::from_individuals(lconv, gconv);
                }
            } else {
                // subtype rank equal
                // promotion rank not yet equal

                // calculate promotion offsets to equalize promotion rank
                auto const [lesser_p_off, greater_p_off] = lesser_p_rank < greater_p_rank
                                                                   ? std::make_pair(0ul, greater_p_rank - lesser_p_rank)
                                                                   : std::make_pair(lesser_p_rank - greater_p_rank, 0ul);

                RuntimeConversionEntry const &lconv = lesser.conversion_at_index(lesser_s_off, lesser_p_off);
                RuntimeConversionEntry const &gconv = greater.conversion_at_index(greater_s_off, greater_p_off);

                if (lconv.target_type_iri == gconv.target_type_iri) {
                    // correct conversion found
                    return DatatypeConverter::from_individuals(lconv, gconv);
                }
            }

            lesser_s_off += 1;
            greater_s_off += 1;
        }

        // no conversion available
        return std::nullopt;
    };

    auto const lhs_entry = get_entry(lhs_type_iri);
    assert(lhs_entry != nullptr);
    auto const &lhs_conv = lhs_entry->conversion_table;

    auto const &rhs_entry = get_entry(rhs_type_iri);
    assert(rhs_entry != nullptr);
    auto const &rhs_conv = rhs_entry->conversion_table;

    // call find_conv_impl with entries in correct order (lesser s rank, greater s rank)
    if (lhs_conv.subtype_rank() < rhs_conv.subtype_rank()) {
        return find_conv_impl(lhs_conv, rhs_conv);
    } else {
        auto res = find_conv_impl(rhs_conv, lhs_conv);

        if (res.has_value()) {
            // swap functions to reverse the ordering change
            std::swap(res->convert_lhs, res->convert_rhs);
        }

        return res;
    }
}

}  // namespace rdf4cpp::rdf::datatypes::registry
#endif  //RDF4CPP_DATATYPEREGISTRY_HPP
