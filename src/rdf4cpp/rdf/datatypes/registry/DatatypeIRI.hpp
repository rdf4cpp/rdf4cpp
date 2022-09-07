#ifndef RDF4CPP_REGISTRY_DATATYPEIRI_HPP
#define RDF4CPP_REGISTRY_DATATYPEIRI_HPP

#include <variant>
#include <cstdint>
#include <functional>

#include <rdf4cpp/rdf/storage/node/identifier/LiteralType.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<typename F, typename O>
struct DatatypeIRIVisitor {
    F map_fixed;
    O map_other;

    auto operator()(storage::node::identifier::LiteralType const fixed) -> std::invoke_result_t<F, storage::node::identifier::LiteralType> {
        return std::invoke(this->map_fixed, fixed);
    }

    template<typename S>
    auto operator()(S &&other) -> std::invoke_result_t<O, decltype(std::forward<S>(other))> {
        return std::invoke(this->map_other, std::forward<S>(other));
    }
};

template<typename F, typename O>
DatatypeIRIVisitor(F, O) -> DatatypeIRIVisitor<F, O>;

struct DatatypeIRIView {
private:
    using variant_t = std::variant<storage::node::identifier::LiteralType, std::string_view>;
    variant_t inner;
public:
    explicit constexpr DatatypeIRIView(storage::node::identifier::LiteralType fixed)
        : inner{fixed} {
    }

    explicit constexpr DatatypeIRIView(std::string_view const other)
        : inner{other} {
    }

    [[nodiscard]] constexpr bool is_fixed() const {
        return this->inner.index() == 0;
    }

    [[nodiscard]] constexpr bool is_dynamic() const noexcept {
        return this->inner.index() == 1;
    }

    [[nodiscard]] constexpr storage::node::identifier::LiteralType get_fixed() const {
        return std::get<0>(this->inner);
    }

    [[nodiscard]] constexpr std::string_view get_dynamic() const {
        return std::get<1>(this->inner);
    }

    constexpr std::strong_ordering operator<=>(DatatypeIRIView const &other) const noexcept = default;

    template<typename F>
    friend constexpr decltype(auto) visit(F &&f, DatatypeIRIView const &self) noexcept(noexcept(std::visit(std::forward<F>(f), std::declval<variant_t const &>()))) {
        return std::visit(std::forward<F>(f), self.inner);
    }
};


struct DatatypeIRI {
private:
    using variant_t = std::variant<storage::node::identifier::LiteralType, std::string>;
    variant_t inner;
public:
    explicit inline DatatypeIRI(DatatypeIRIView const iri)
        : inner{visit(
                  DatatypeIRIVisitor{
                          [](storage::node::identifier::LiteralType fixed) { return variant_t{fixed}; },
                          [](std::string_view other) { return variant_t{std::string{other}}; }},
                  iri)} {
    }

    explicit inline DatatypeIRI(storage::node::identifier::LiteralType const fixed)
        : inner{fixed} {
    }

    explicit inline DatatypeIRI(std::string const &other)
        : inner{other} {
    }

    explicit inline DatatypeIRI(std::string &&other)
        : inner{std::move(other)} {
    }

    inline operator DatatypeIRIView() const noexcept {
        return visit(DatatypeIRIVisitor{
                             [](storage::node::identifier::LiteralType fixed) { return DatatypeIRIView{fixed}; },
                             [](std::string const &other) { return DatatypeIRIView{other}; }},
                     this->inner);
    }

    [[nodiscard]] inline bool is_fixed() const {
        return this->inner.index() == 0;
    }

    [[nodiscard]] inline bool is_dynamic() const noexcept {
        return this->inner.index() == 1;
    }

    [[nodiscard]] inline storage::node::identifier::LiteralType get_fixed() const {
        return std::get<0>(this->inner);
    }

    [[nodiscard]] inline std::string const &get_dynamic() const {
        return std::get<1>(this->inner);
    }

    inline std::strong_ordering operator<=>(DatatypeIRI const &other) const noexcept = default;

    template<typename F>
    friend decltype(auto) visit(F && f, DatatypeIRI const &self) noexcept(noexcept(std::visit(std::forward<F>(f), std::declval<variant_t const &>()))) {
        return std::visit(std::forward<F>(f), self);
    }
};

} // rdf4cpp::rdf::datatypes::registry

#endif  //RDF4CPP_REGISTRY_DATATYPEIRI_HPP
