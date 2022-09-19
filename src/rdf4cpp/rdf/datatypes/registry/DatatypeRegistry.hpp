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
#include <variant>
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
        nonstd::expected<std::any, NumericOpError> result_value;
    };

    using unop_fptr_t = NumericOpResult (*)(std::any const &);
    using binop_fptr_t = NumericOpResult (*)(std::any const &, std::any const &);

    using compare_fptr_t = std::partial_ordering (*)(std::any const &, std::any const &);

    struct NumericOpsImpl {
        binop_fptr_t add_fptr;  // a + b
        binop_fptr_t sub_fptr;  // a - b
        binop_fptr_t mul_fptr;  // a * b
        binop_fptr_t div_fptr;  // a / b

        unop_fptr_t pos_fptr;  // +a
        unop_fptr_t neg_fptr;  // -a
    };

    struct NumericOpsStub {
        /**
         * Determines the number of subtype levels to immediately skip up in
         * the hierarchy, if searching for a _numeric_ type conversion.
         *
         * @example if start_s_off = 1 then the search will _start_ at the immediate supertype
         *      of the type this NumericOpsStub belongs to.
         */
        size_t start_s_off;
    };

    struct NumericOps : std::variant<NumericOpsStub, NumericOpsImpl> {
        [[nodiscard]] constexpr bool is_stub() const noexcept {
            return this->index() == 0;
        }

        [[nodiscard]] constexpr bool is_impl() const noexcept {
            return this->index() == 1;
        }

        [[nodiscard]] constexpr NumericOpsStub const &get_stub() const noexcept {
            return std::get<NumericOpsStub>(*this);
        }

        [[nodiscard]] constexpr NumericOpsImpl const &get_impl() const noexcept {
            return std::get<NumericOpsImpl>(*this);
        }
    };

    struct DatatypeEntry {
        std::string datatype_iri;         // datatype IRI string
        factory_fptr_t factory_fptr;      // construct from string
        to_string_fptr_t to_string_fptr;  // convert to string

        ebv_fptr_t ebv_fptr; // convert to effective boolean value

        std::optional<NumericOps> numeric_ops;
        compare_fptr_t compare_fptr;

        RuntimeConversionTable conversion_table;

        inline static DatatypeEntry for_search(std::string_view const datatype_iri) {
            return DatatypeEntry{
                    .datatype_iri = std::string{datatype_iri},
                    .factory_fptr = nullptr,
                    .to_string_fptr = nullptr,
                    .ebv_fptr = nullptr,
                    .numeric_ops = std::nullopt,
                    .compare_fptr = nullptr,
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
    template<datatypes::NumericImplLiteralDatatype datatype_info>
    static NumericOpsImpl make_numeric_ops_impl();

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
     * Get a factory_fptr_t for a datatype IRI std::string. The factory_fptr_t can be used like `std::any type_instance = factory_fptr("types string representation")`.
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
    inline static NumericOps const *get_numerical_ops(std::string_view const datatype_iri) {
        auto const res = find_map_entry(datatype_iri, [](auto const &entry) {
            return &entry.numeric_ops;
        });

        // res is nullopt if no datatype matching given datatype_iri was found
        if (res.has_value()) {
            // contained ptr cannot be nullptr as by return in lambda for find_map_entry above
            // optional behind contained ptr can be nullopt if type is not numeric
            if (auto const ops_ptr = res.value(); ops_ptr->has_value()) {
                return &ops_ptr->value();
            }
        }

        // no datatype found or not numeric
        return nullptr;
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

    inline static compare_fptr_t get_compare(std::string_view const datatype_iri) {
        auto const res = find_map_entry(datatype_iri, [](auto const &entry) {
            return entry.compare_fptr;
        });

        return res.has_value() ? *res : nullptr;
    }

    /**
     * Tries to find a conversion to a common type in the conversion tables lhs_conv and rhs_conv.
     *
     * @return the found conversion if there is a viable one
     */
    static std::optional<DatatypeConverter> get_common_type_conversion(RuntimeConversionTable const &lhs_conv, RuntimeConversionTable const &rhs_conv, size_t lhs_init_soff = 0, size_t rhs_init_soff = 0);

    /**
     * Tries to find a conversion to a common type in the context of numeric operations.
     * This function is similar to get_common_type_conversion except that it is aware of numeric-stubs
     * and will always return a conversion to a numeric-impl or none at all.
     *
     * @note must be called with datatypes that are numeric
     * @return A conversion to a common type that is also a numeric-impl if there is a viable one
     */
    inline static std::optional<DatatypeConverter> get_common_numeric_op_type_conversion(DatatypeEntry const &lhs_entry, DatatypeEntry const &rhs_entry) {
        assert(lhs_entry.numeric_ops.has_value());
        assert(rhs_entry.numeric_ops.has_value());
        
        size_t const lhs_init_soff = lhs_entry.numeric_ops->is_stub() ? lhs_entry.numeric_ops->get_stub().start_s_off : 0;
        size_t const rhs_init_soff = rhs_entry.numeric_ops->is_stub() ? rhs_entry.numeric_ops->get_stub().start_s_off : 0;

        return get_common_type_conversion(lhs_entry.conversion_table, rhs_entry.conversion_table, lhs_init_soff, rhs_init_soff);
    }

    /**
     * Returns the conversion that turns a value for a numeric-stub DatatypeEntry into
     * a value of it's corresponding impl-type.
     *
     * @note must be called with a datatype that is stub-numeric
     * @return the conversion to the corresponding impl-type
     */
    inline static RuntimeConversionEntry const &get_numeric_op_impl_conversion(DatatypeEntry const &entry) {
        assert(entry.numeric_ops.has_value());
        assert(entry.numeric_ops->is_stub());

        return entry.conversion_table.conversion_at_index(entry.numeric_ops->get_stub().start_s_off, 0);
    }

    /**
     * Tries to find a conversion to convert lhs_type_iri and rhs_type_iri into a
     * common type to be used in e.g. numeric calculations.
     *
     * @return nullopt if any of lhs_type_iri or rhs_type_iri does not have a datatype registered
     * or there is no viable conversion, else the found conversion
     */
    inline static std::optional<DatatypeConverter> get_common_type_conversion(std::string_view lhs_type_iri, std::string_view rhs_type_iri) {
        auto const lhs_entry = get_entry(lhs_type_iri);
        if (lhs_entry == nullptr) {
            return std::nullopt;
        }

        auto const &rhs_entry = get_entry(rhs_type_iri);
        if (rhs_entry == nullptr) {
            return std::nullopt;
        }

        return get_common_type_conversion(lhs_entry->conversion_table, rhs_entry->conversion_table);
    }
};


/**
 * To register a datatype, at least a LiteralDatatypeImpl instantiation must be provided.
 * If LiteralDatatype_t::cpp_type does not overload `operator<<`, the to_string(const datatype_t &value) must be specialized.
 * @tparam LiteralDatatype_t datatype that is being registered
 */
template<datatypes::LiteralDatatype LiteralDatatype_t>
inline void DatatypeRegistry::add() {
    using conversion_table_t = decltype(make_conversion_table_for<LiteralDatatype_t>());

    auto const num_ops = []() -> std::optional<NumericOps> {
        if constexpr (datatypes::NumericImpl<LiteralDatatype_t>) {
            return NumericOps{make_numeric_ops_impl<LiteralDatatype_t>()};
        } else if constexpr (datatypes::NumericStub<LiteralDatatype_t>) {
            constexpr auto soff = conversion_detail::calculate_subtype_offset<typename LiteralDatatype_t::numeric_impl_type, conversion_table_t>();
            static_assert(soff.has_value(), "a stub-numeric type must define linearly reachable supertype that is impl-numeric as numeric_impl_type");

            return NumericOps{NumericOpsStub{.start_s_off = *soff}};
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

    auto const compare_fptr = []() -> compare_fptr_t {
        if constexpr (datatypes::ComparableLiteralDatatype<LiteralDatatype_t>) {
            return [](std::any const &lhs, std::any const &rhs) -> std::partial_ordering {
                auto const &lhs_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(lhs);
                auto const &rhs_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(rhs);

                return LiteralDatatype_t::compare(lhs_val, rhs_val);
            };
        } else {
            return nullptr;
        }
    }();

    DatatypeRegistry::add(DatatypeEntry {
            .datatype_iri = std::string{LiteralDatatype_t::identifier},
            .factory_fptr = [](std::string_view string_repr) -> std::any {
                return std::any(LiteralDatatype_t::from_string(string_repr));
            },
            .to_string_fptr = [](const std::any &value) -> std::string {
                return LiteralDatatype_t::to_string(std::any_cast<typename LiteralDatatype_t::cpp_type>(value));
            },
            .ebv_fptr = ebv_fptr,
            .numeric_ops = num_ops,
            .compare_fptr = compare_fptr,
            .conversion_table = RuntimeConversionTable::from_concrete<conversion_table_t>()});
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

template<typename T>
[[nodiscard]] nonstd::expected<std::any, NumericOpError> map_expected(nonstd::expected<T, NumericOpError> const &e) noexcept {
    if (e.has_value()) {
        return *e;
    } else {
        return nonstd::make_unexpected(e.error());
    }
}

}  // namespace detail

template<datatypes::NumericImplLiteralDatatype LiteralDatatype_t>
inline DatatypeRegistry::NumericOpsImpl DatatypeRegistry::make_numeric_ops_impl() {
    return NumericOpsImpl{
            // a + b
            [](std::any const &lhs, std::any const &rhs) -> NumericOpResult {
                auto const &lhs_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(lhs);
                auto const &rhs_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(rhs);

                return NumericOpResult{
                        .result_type_iri = detail::SelectOpRes<typename LiteralDatatype_t::add_result, LiteralDatatype_t>::type::identifier,
                        .result_value = detail::map_expected(LiteralDatatype_t::add(lhs_val, rhs_val))};
            },
            // a - b
            [](std::any const &lhs, std::any const &rhs) -> NumericOpResult {
                auto const &lhs_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(lhs);
                auto const &rhs_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(rhs);

                return NumericOpResult{
                        .result_type_iri = detail::SelectOpRes<typename LiteralDatatype_t::sub_result, LiteralDatatype_t>::type::identifier,
                        .result_value = detail::map_expected(LiteralDatatype_t::sub(lhs_val, rhs_val))};
            },
            // a * b
            [](std::any const &lhs, std::any const &rhs) -> NumericOpResult {
                auto const &lhs_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(lhs);
                auto const &rhs_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(rhs);

                return NumericOpResult{
                        .result_type_iri = detail::SelectOpRes<typename LiteralDatatype_t::mul_result, LiteralDatatype_t>::type::identifier,
                        .result_value = detail::map_expected(LiteralDatatype_t::mul(lhs_val, rhs_val))};
            },
            // a / b
            [](std::any const &lhs, std::any const &rhs) -> NumericOpResult {
                auto const &lhs_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(lhs);
                auto const &rhs_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(rhs);

                return NumericOpResult{
                        .result_type_iri = detail::SelectOpRes<typename LiteralDatatype_t::div_result, LiteralDatatype_t>::type::identifier,
                        .result_value = detail::map_expected(LiteralDatatype_t::div(lhs_val, rhs_val))};
            },
            // +a
            [](std::any const &operand) -> NumericOpResult {
                auto const &operand_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(operand);

                return NumericOpResult{
                        .result_type_iri = detail::SelectOpRes<typename LiteralDatatype_t::pos_result, LiteralDatatype_t>::type::identifier,
                        .result_value = detail::map_expected(LiteralDatatype_t::pos(operand_val))};
            },
            // -a
            [](std::any const &operand) -> NumericOpResult {
                auto const &operand_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(operand);

                return NumericOpResult{
                        .result_type_iri = detail::SelectOpRes<typename LiteralDatatype_t::neg_result, LiteralDatatype_t>::type::identifier,
                        .result_value = detail::map_expected(LiteralDatatype_t::neg(operand_val))};
            }};
}

inline std::optional<DatatypeRegistry::DatatypeConverter> DatatypeRegistry::get_common_type_conversion(
        RuntimeConversionTable const &lhs_conv,
        RuntimeConversionTable const &rhs_conv,
        size_t const lhs_init_soff,
        size_t const rhs_init_soff) {

    auto const find_conv_impl = [](RuntimeConversionTable const &lesser, RuntimeConversionTable const &greater,
                                   size_t const lesser_init_soff, size_t const greater_init_soff) -> std::optional<DatatypeConverter> {
        auto const lesser_s_rank = lesser.subtype_rank() - lesser_init_soff;
        auto const greater_s_rank = greater.subtype_rank() - greater_init_soff;

        // lesser should be the conversion table of the type with lower subtype rank
        assert(lesser_s_rank <= greater_s_rank);

        // calculate initial subtype offsets to equalize subtype rank
        size_t lesser_s_off = lesser_init_soff;
        size_t greater_s_off = greater_init_soff + greater_s_rank - lesser_s_rank;

        while (lesser_s_off < lesser.subtype_rank() && greater_s_off < greater.subtype_rank()) {
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

    // call find_conv_impl with entries in correct order (lesser s rank, greater s rank)
    if (lhs_conv.subtype_rank() - lhs_init_soff < rhs_conv.subtype_rank() - rhs_init_soff) {
        return find_conv_impl(lhs_conv, rhs_conv, lhs_init_soff, rhs_init_soff);
    } else {
        auto res = find_conv_impl(rhs_conv, lhs_conv, rhs_init_soff, lhs_init_soff);

        if (res.has_value()) {
            // swap functions to reverse the ordering change
            std::swap(res->convert_lhs, res->convert_rhs);
        }

        return res;
    }
}

}  // namespace rdf4cpp::rdf::datatypes::registry
#endif  //RDF4CPP_DATATYPEREGISTRY_HPP
