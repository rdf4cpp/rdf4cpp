#include <rdf4cpp/rdf/datatypes/rdf/LangString.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template struct LiteralDatatypeImpl<rdf_lang_string,
                                    capabilities::Comparable,
                                    capabilities::FixedId>;

}  // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::registry::lang_tags {
#ifdef CONST_LANG_TAG_INLINES
const
#endif
        std::vector<std::string>
                tags_to_inline{
                        "de",
                        "en",
                        "fr",
                        "ch",
                };

size_t inlined_size() noexcept {
    // number of bits needed for the biggest id
    return static_cast<size_t>(std::floor(std::log2(static_cast<double>(tags_to_inline.size() - 1)))) + 1;
}

uint64_t shift() noexcept {
    return storage::node::identifier::LiteralID::width - inlined_size();
}

uint64_t mask_inlined() noexcept {
    return ((1l << inlined_size()) - 1) << shift();
}

uint64_t mask_base_id() noexcept {
    return (1l << shift()) - 1;
}

std::string_view inlined_to_tag(LangTagID id) noexcept {
    if (id < tags_to_inline.size())
        return tags_to_inline[id];
    return "";
}

std::optional<LangTagID> tag_to_inlined(std::string_view tag) noexcept {
    for (uint64_t i = 0; i < tags_to_inline.size(); ++i) {
        if (tags_to_inline[i] == tag) {
            return i;
        }
    }
    return std::nullopt;
}

std::optional<storage::node::identifier::LiteralID> try_into_inlined(storage::node::identifier::LiteralID id, LangTagID tag) noexcept {
    if ((id.value & mask_inlined()) != 0)
        return std::nullopt;
    return storage::node::identifier::LiteralID{id.value | (tag << shift())};
}

std::pair<LangTagID, storage::node::identifier::LiteralID> from_inlined(storage::node::identifier::LiteralID id) noexcept {
    uint64_t t = (id.value & mask_inlined()) >> shift();
    uint64_t i = id.value & mask_base_id();
    return std::pair{t, storage::node::identifier::LiteralID{i}};
}
}  // namespace rdf4cpp::rdf::datatypes::registry::lang_tags
