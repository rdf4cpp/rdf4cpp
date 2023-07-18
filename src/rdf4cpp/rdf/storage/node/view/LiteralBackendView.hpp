#ifndef RDF4CPP_LITERALBACKENDHANDLE_HPP
#define RDF4CPP_LITERALBACKENDHANDLE_HPP

#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>
#include <rdf4cpp/rdf/util/Overloaded.hpp>

#include <dice/hash.hpp>

#include <any>
#include <string>
#include <string_view>


namespace rdf4cpp::rdf::storage::node::view {

struct LexicalFormLiteralBackendView {
    identifier::NodeID datatype_id;
    std::string_view lexical_form;
    std::string_view language_tag;

    bool operator==(LexicalFormLiteralBackendView const &other) const noexcept = default;
    [[nodiscard]] size_t hash() const noexcept;
};

struct ValueLiteralBackendView {
    identifier::LiteralType datatype;
    std::any value;

    template<datatypes::FixedIdLiteralDatatype Contained>
    [[nodiscard]] bool eq(typename Contained::cpp_type const &other) const noexcept {
        return *std::any_cast<typename Contained::cpp_type>(&value) == other;
    }

    template<datatypes::FixedIdLiteralDatatype Contained>
    [[nodiscard]] size_t hash() const noexcept {
        return dice::hash::dice_hash_templates<::dice::hash::Policies::wyhash>::dice_hash(*std::any_cast<typename Contained::cpp_type>(&value));
    }
};

/**
 * A LiteralBackendView is either a LexicalFormLiteralBackendView
 * or a ValueLiteralBackendView.
 */
struct LiteralBackendView {
private:
    std::variant<LexicalFormLiteralBackendView, ValueLiteralBackendView> inner;

    template<typename Self, typename ...Fs>
    static decltype(auto) visit_impl(Self &&self, Fs &&...fs) {
        return std::visit(rdf::util::Overloaded{std::forward<Fs>(fs)...}, std::forward<Self>(self).inner);
    }
public:
    LiteralBackendView(ValueLiteralBackendView const &any);
    LiteralBackendView(ValueLiteralBackendView &&any) noexcept;
    LiteralBackendView(LexicalFormLiteralBackendView const &lexical) noexcept;

    [[nodiscard]] bool is_lexical() const noexcept;
    [[nodiscard]] bool is_value() const noexcept;

    [[nodiscard]] LexicalFormLiteralBackendView const &get_lexical() const;
    [[nodiscard]] ValueLiteralBackendView const &get_value() const &;
    [[nodiscard]] ValueLiteralBackendView &&get_value() &&;

    template<typename ...Fs>
    decltype(auto) visit(Fs &&...fs) const & {
        return visit_impl(*this, std::forward<Fs>(fs)...);
    }

    template<typename ...Fs>
    decltype(auto) visit(Fs &&...fs) & {
        return visit_impl(*this, std::forward<Fs>(fs)...);
    }

    template<typename ...Fs>
    decltype(auto) visit(Fs &&...fs) && {
        return visit_impl(std::move(*this), std::forward<Fs>(fs)...);
    }

    template<typename ...Fs>
    decltype(auto) visit(Fs &&...fs) const && {
        return visit_impl(*this, std::forward<Fs>(fs)...);
    }
};

}  // namespace rdf4cpp::rdf::storage::node::view

#endif  //RDF4CPP_LITERALBACKENDHANDLE_HPP
