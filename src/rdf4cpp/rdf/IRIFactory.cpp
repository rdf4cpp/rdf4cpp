#include "IRIFactory.hpp"

namespace rdf4cpp::rdf {

IRIFactory::IRIFactory(std::string_view base) noexcept  : base(base) {
}

nonstd::expected<IRI, IRIFactoryError> IRIFactory::from_relative(std::string_view rel, storage::node::NodeStorage &storage) const noexcept {
    std::string iri = base;
    iri.append(rel);
    return create_and_validate(iri, storage);
}

nonstd::expected<IRI, IRIFactoryError> IRIFactory::from_absolute_prefix(std::string_view prefix, std::string_view local, storage::node::NodeStorage &storage) const {
    std::string pre{prefix};
    auto i = prefixes.find(pre);
    if (i == prefixes.end())
        return nonstd::make_unexpected(IRIFactoryError::UnknownPrefix);
    pre = i->second;
    pre.append(local);
    return create_and_validate(pre, storage);
}

nonstd::expected<IRI, IRIFactoryError> IRIFactory::from_relative_prefix(std::string_view prefix, std::string_view local, storage::node::NodeStorage &storage) const {
    std::string pre{prefix};
    auto i = prefixes.find(pre);
    if (i == prefixes.end())
        return nonstd::make_unexpected(IRIFactoryError::UnknownPrefix);
    pre = base;
    pre.append(i->second);
    pre.append(local);
    return create_and_validate(pre, storage);
}

nonstd::expected<IRI, IRIFactoryError> IRIFactory::create_and_validate(std::string_view iri, storage::node::NodeStorage &storage) const noexcept {
    return IRI{iri, storage};
}

void IRIFactory::assign_prefix(std::string_view prefix, std::string_view expanded) {
    std::string pre{prefix};
    prefixes[pre] = expanded;
}
void IRIFactory::clear_prefix(std::string_view prefix) {
    std::string pre{prefix};
    prefixes.erase(pre);
}

}  // namespace rdf4cpp::rdf
