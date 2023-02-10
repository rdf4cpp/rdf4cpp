#ifndef RDF4CPP_LITERALBACKENDHANDLE_HPP
#define RDF4CPP_LITERALBACKENDHANDLE_HPP

#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>

#include <string>
#include <string_view>

#include <rdf4cpp/rdf/util/Any.hpp>
#include <rdf4cpp/rdf/util/Overloaded.hpp>

namespace rdf4cpp::rdf::storage::node::view {

struct LexicalFormLiteralBackendView {
    identifier::NodeID datatype_id;
    std::string_view lexical_form;
    std::string_view language_tag;

    bool operator==(LexicalFormLiteralBackendView const &) const noexcept = default;
    [[nodiscard]] size_t hash() const noexcept;
};

struct ValueLiteralBackendView {
    identifier::LiteralType datatype;
    rdf::util::Any value;
};

struct LiteralBackendView {
    template<typename Self, typename ...Fs>
    friend decltype(auto) visit(Self &&self, Fs &&...fs);

private:
    std::variant<LexicalFormLiteralBackendView, ValueLiteralBackendView> inner;

public:
    LiteralBackendView(ValueLiteralBackendView const &any);
    LiteralBackendView(ValueLiteralBackendView &&any) noexcept;
    LiteralBackendView(LexicalFormLiteralBackendView const &lexical) noexcept;

    [[nodiscard]] bool is_lexical() const noexcept;
    [[nodiscard]] bool is_value() const noexcept;

    [[nodiscard]] LexicalFormLiteralBackendView const &get_lexical() const;
    [[nodiscard]] ValueLiteralBackendView const &get_value() const &;
    [[nodiscard]] ValueLiteralBackendView &&get_value() &&;
};

template<typename Self, typename ...Fs>
decltype(auto) visit(Self &&self, Fs &&...fs) {
    return std::visit(rdf::util::Overloaded{std::forward<Fs>(fs)...}, std::forward<Self>(self).inner);
}

}  // namespace rdf4cpp::rdf::storage::node::view

#endif  //RDF4CPP_LITERALBACKENDHANDLE_HPP
