#include <rdf4cpp/rdf/datatypes/rdf/LangString.hpp>

#include <rdf4cpp/rdf/datatypes/registry/util/LangTag.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

std::weak_ordering registry::LangStringRepr::operator<=>(LangStringRepr const &other) const noexcept {
    if (auto const lfc = this->lexical_form <=> other.lexical_form; lfc != std::strong_ordering::equal) {
        return lfc;
    }

    return util::LangTagView{this->language_tag.data(), this->language_tag.size()} <=> util::LangTagView{other.language_tag.data(), other.language_tag.size()};
}

template struct LiteralDatatypeImpl<rdf_lang_string,
                                    capabilities::Comparable,
                                    capabilities::FixedId>;

} // namespace rdf4cpp::rdf::datatypes::registry
