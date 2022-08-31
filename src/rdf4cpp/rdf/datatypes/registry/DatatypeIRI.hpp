#ifndef RDF4CPP_REGISTRY_DATATYPEIRI_HPP
#define RDF4CPP_REGISTRY_DATATYPEIRI_HPP

#include <variant>
#include <cstdint>


namespace rdf4cpp::rdf::datatypes::registry {

template<typename F, typename O>
struct DatatypeIRIVisitor {
    F map_fixed;
    O map_other;

    auto operator()(uint8_t const fixed) -> std::invoke_result_t<F, uint8_t> {
        return std::invoke(this->map_fixed, fixed);
    }

    template<typename S>
    auto operator()(S &&other) -> std::invoke_result_t<O, decltype(std::forward<S>(other))> {
        return std::invoke(this->map_other, std::forward<S>(other));
    }
};

template<typename F, typename O>
DatatypeIRIVisitor(F, O) -> DatatypeIRIVisitor<F, O>;

struct DatatypeIRIView : std::variant<uint8_t, std::string_view> {
    using variant_t = std::variant<uint8_t, std::string_view>;

    explicit constexpr DatatypeIRIView(uint8_t fixed)
        : variant_t{fixed} {
    }

    explicit constexpr DatatypeIRIView(std::string_view const other)
        : variant_t{other} {
    }

    [[nodiscard]] constexpr bool is_fixed() const {
        return this->index() == 0;
    }

    [[nodiscard]] constexpr bool is_other() const noexcept {
        return this->index() == 1;
    }

    [[nodiscard]] constexpr uint8_t get_fixed() const {
        return std::get<uint8_t>(*this);
    }

    [[nodiscard]] constexpr std::string_view get_other() const {
        return std::get<std::string_view>(*this);
    }

    std::strong_ordering operator<=>(DatatypeIRIView const &other) const noexcept = default;
};


struct DatatypeIRI : std::variant<uint8_t, std::string> {
    using variant_t = std::variant<uint8_t, std::string>;

    explicit constexpr DatatypeIRI(DatatypeIRIView const iri)
        : variant_t{visit(
                  DatatypeIRIVisitor{
                          [](uint8_t fixed) { return variant_t{fixed}; },
                          [](std::string_view other) { return variant_t{std::string{other}}; }},
                  iri)} {
    }

    explicit constexpr DatatypeIRI(uint8_t const fixed)
        : variant_t{fixed} {
    }

    explicit constexpr DatatypeIRI(std::string const &other)
        : variant_t{std::in_place_type<std::string>, other} {
    }

    explicit constexpr DatatypeIRI(std::string &&other)
        : variant_t{std::in_place_type<std::string>, std::move(other)} {
    }

    constexpr operator DatatypeIRIView() const noexcept {
        return visit(DatatypeIRIVisitor{
                             [](uint8_t fixed) { return DatatypeIRIView{fixed}; },
                             [](std::string const &other) { return DatatypeIRIView{other}; }},
                     *this);
    }

    static constexpr DatatypeIRI empty() {
        return DatatypeIRI{""};
    }

    [[nodiscard]] constexpr bool is_fixed() const {
        return this->index() == 0;
    }

    [[nodiscard]] constexpr bool is_other() const noexcept {
        return this->index() == 1;
    }

    [[nodiscard]] constexpr uint8_t get_fixed() const {
        return std::get<uint8_t>(*this);
    }

    [[nodiscard]] constexpr std::string const &get_other() const {
        return std::get<std::string>(*this);
    }

    constexpr std::strong_ordering operator<=>(DatatypeIRI const &other) const noexcept = default;

    constexpr bool operator==(std::string_view const other) const noexcept {
        return this->is_other() && this->get_other() == other;
    }

    constexpr bool operator==(uint8_t const fixed) const noexcept {
        return this->is_fixed() && this->get_fixed() == fixed;
    }
};

} // rdf4cpp::rdf::datatypes::registry

#endif  //RDF4CPP_REGISTRY_DATATYPEIRI_HPP
