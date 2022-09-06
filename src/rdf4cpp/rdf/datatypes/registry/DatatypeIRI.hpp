#ifndef RDF4CPP_REGISTRY_DATATYPEIRI_HPP
#define RDF4CPP_REGISTRY_DATATYPEIRI_HPP

#include <variant>
#include <cstdint>

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

struct DatatypeIRIView : std::variant<storage::node::identifier::LiteralType, std::string_view> {
    using variant_t = std::variant<storage::node::identifier::LiteralType, std::string_view>;

    explicit constexpr DatatypeIRIView(storage::node::identifier::LiteralType fixed)
        : variant_t{fixed} {
    }

    explicit constexpr DatatypeIRIView(std::string_view const other)
        : variant_t{other} {
    }

    [[nodiscard]] constexpr bool is_fixed() const {
        return this->index() == 0;
    }

    [[nodiscard]] constexpr bool is_dynamic() const noexcept {
        return this->index() == 1;
    }

    [[nodiscard]] constexpr storage::node::identifier::LiteralType get_fixed() const {
        return std::get<0>(*this);
    }

    [[nodiscard]] constexpr std::string_view get_other() const {
        return std::get<1>(*this);
    }

    std::strong_ordering operator<=>(DatatypeIRIView const &other) const noexcept = default;
};


struct DatatypeIRI : std::variant<storage::node::identifier::LiteralType, std::string> {
    using variant_t = std::variant<storage::node::identifier::LiteralType, std::string>;

    explicit inline DatatypeIRI(DatatypeIRIView const iri)
        : variant_t{visit(
                  DatatypeIRIVisitor{
                          [](storage::node::identifier::LiteralType fixed) { return variant_t{fixed}; },
                          [](std::string_view other) { return variant_t{std::string{other}}; }},
                  iri)} {
    }

    explicit inline DatatypeIRI(storage::node::identifier::LiteralType const fixed)
        : variant_t{fixed} {
    }

    explicit inline DatatypeIRI(std::string const &other)
        : variant_t{other} {
    }

    explicit inline DatatypeIRI(std::string &&other)
        : variant_t{std::move(other)} {
    }

    inline operator DatatypeIRIView() const noexcept {
        return visit(DatatypeIRIVisitor{
                             [](storage::node::identifier::LiteralType fixed) { return DatatypeIRIView{fixed}; },
                             [](std::string const &other) { return DatatypeIRIView{other}; }},
                     *this);
    }

    [[nodiscard]] inline bool is_fixed() const {
        return this->index() == 0;
    }

    [[nodiscard]] inline bool is_dynamic() const noexcept {
        return this->index() == 1;
    }

    [[nodiscard]] inline storage::node::identifier::LiteralType get_fixed() const {
        return std::get<0>(*this);
    }

    [[nodiscard]] inline std::string const &get_other() const {
        return std::get<1>(*this);
    }

    inline std::strong_ordering operator<=>(DatatypeIRI const &other) const noexcept = default;

    inline bool operator==(std::string_view const other) const noexcept {
        return this->is_dynamic() && this->get_other() == other;
    }

    inline bool operator==(storage::node::identifier::LiteralType const fixed) const noexcept {
        return this->is_fixed() && this->get_fixed() == fixed;
    }
};

} // rdf4cpp::rdf::datatypes::registry

#endif  //RDF4CPP_REGISTRY_DATATYPEIRI_HPP
