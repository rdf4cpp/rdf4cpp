#include "IRIFactory.hpp"

namespace rdf4cpp::rdf {

IRIFactory::IRIFactory(std::string_view base) noexcept  : base(base) {
}

nonstd::expected<IRI, IRIFactoryError> IRIFactory::from_relative(std::string_view rel, storage::node::NodeStorage &storage) const noexcept {
    std::string iri = base;
    iri.append(rel);
    return create_and_validate(iri, storage);
}

nonstd::expected<IRI, IRIFactoryError> IRIFactory::from_prefix(std::string_view prefix, std::string_view local, storage::node::NodeStorage &storage) const {
    auto i = prefixes.find(prefix);
    if (i == prefixes.end())
        return nonstd::make_unexpected(IRIFactoryError::UnknownPrefix);
    std::string deref {i->second};
    deref.append(local);
    if (is_relative(deref))
        return from_relative(deref, storage);
    return create_and_validate(deref, storage);
}

nonstd::expected<IRI, IRIFactoryError> IRIFactory::create_and_validate(std::string_view iri, storage::node::NodeStorage &storage) const noexcept {
    return IRI{iri, storage};
}

void IRIFactory::assign_prefix(std::string_view prefix, std::string_view expanded) {
    std::string pre{prefix};
    prefixes[pre] = expanded;
}
void IRIFactory::clear_prefix(std::string_view prefix) {
    prefixes.erase(prefix);
}
bool IRIFactory::is_relative(std::string_view iri) noexcept {
    auto c = iri.find(':');
    if (c == std::string_view::npos) // no : -> relative
        return true;
    auto s = iri.find('/');
    return c > s; // :/ -> absolute, /: -> relative
}

}  // namespace rdf4cpp::rdf
