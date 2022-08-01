#ifndef RDF4CPP_UTIL_HPP
#define RDF4CPP_UTIL_HPP

#include <tuple>
#include <type_traits>
#include <utility>

namespace rdf4cpp::rdf::datatypes::registry::util {

namespace tuple_util_detail {

/**
 * tuple map implementation
 */
template<typename F, typename TupleLike, size_t ...Ixs>
constexpr auto tuple_map_impl(TupleLike &&tup, F &&f, std::index_sequence<Ixs...>) {
    return std::make_tuple(f(std::get<Ixs>(std::forward<TupleLike>(tup)))...);
}

/**
 * tuple fold implementation
 */
template<typename Acc, typename F, typename TupleLike, size_t ...Ixs>
constexpr Acc tuple_fold_impl(TupleLike &&tup, Acc init, F &&f, std::index_sequence<Ixs...>) {
    ((init = f(std::move(init), std::get<Ixs>(std::forward<TupleLike>(tup)))), ...);
    return init;
}

template<typename TupleLike, typename Acc, typename F, size_t ...Ixs>
constexpr Acc tuple_type_fold_impl(Acc init, F &&f, std::index_sequence<Ixs...>) {
    ((init = f.template operator()<std::tuple_element_t<Ixs, TupleLike>>(std::move(init))), ...);
    return init;
}


} // namespace tuple_util_detail

/**
 * Map each element in a tuple using a unary operation f
 * and store the results in a another tuple.
 *
 * @param tup a std::tuple like object to map over
 * @param f a unary operation to map the elements
 * @return the tuple generated by applying f to each element in tup
 */
template<typename F, typename TupleLike>
constexpr auto tuple_map(TupleLike &&tup, F &&f) {
    constexpr size_t tup_size = std::tuple_size_v<std::remove_cvref_t<TupleLike>>;
    return tuple_util_detail::tuple_map_impl(std::forward<TupleLike>(tup), std::forward<F>(f), std::make_index_sequence<tup_size>{});
}

/**
 * Fold all elements in a tuple into a single object.
 * The provided function f will be repeatedly called as f(acc, element) for each element in the
 * tuple and is expected to return the acc for the next invocation.
 *
 * @tparam Acc the type of the accumulator
 * @param tup a std::tuple like object to fold
 * @param init the initial value of the accumulator
 * @param f a binary operation that will be called with the accumulator and the current element and should return the new accumulator
 * @return the final accumulator value
 */
template<typename Acc, typename F, typename TupleLike>
constexpr Acc tuple_fold(TupleLike &&tup, Acc init, F &&f) {
    constexpr size_t tup_size = std::tuple_size_v<std::remove_cvref_t<TupleLike>>;
    return tuple_util_detail::tuple_fold_impl(std::forward<TupleLike>(tup), std::move(init), std::forward<F>(f), std::make_index_sequence<tup_size>{});
}

/**
 * Simmilar to tuple_fold except that it is not folding over the actual elements
 * of the tuple, but just their types.
 * The provided function f will be repeatedly called as f.template operator()<Element>(acc)
 * for each type in the tuple and is expected to return the accumulator for the next invocation.
 *
 * @example
 * @code
 * using tuple_t = std::tuple<int, double, float>;
 *
 * std::string all_types = tuple_type_fold<tuple_t>(std::string{}, []<typename T>(std::string &&acc) {
 *     return std::move(acc) + typeid(T).name() + " ";
 * });
 * @endcode
 *
 * @tparam TupleLike a std::tuple like type to be folded
 * @tparam Acc the type of the accumulator
 * @param init the initial value of the accumulator
 * @param f a unary operation mapping a accumulator to the next accumulator
 * @return the final accumulator value
 */
template<typename TupleLike, typename Acc, typename F>
constexpr Acc tuple_type_fold(Acc init, F &&f) {
    constexpr size_t tup_size = std::tuple_size_v<std::remove_cvref_t<TupleLike>>;
    return tuple_util_detail::tuple_type_fold_impl<TupleLike>(std::move(init), std::forward<F>(f), std::make_index_sequence<tup_size>{});
}

/**
 * Check if all given types in the Ts parameter pack are equal.
 *
 * @tparam Ts types to check for equality
 */
template<typename ...Ts>
struct AllSame : std::false_type {};

template<>
struct AllSame<> : std::true_type {};

template<typename T, typename ...Ts>
struct AllSame<T, Ts...> : std::bool_constant<(std::is_same_v<T, Ts> && ...)> {};

} // namespace rdf4cpp::rdf::datatypes::registry::util

#endif  //RDF4CPP_UTIL_HPP
