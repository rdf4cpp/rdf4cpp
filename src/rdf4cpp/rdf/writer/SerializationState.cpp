#include "SerializationState.hpp"

namespace rdf4cpp::rdf::writer {

class PrefixDataBuilder {
    template<size_t I>
    static consteval auto build_line() {
        using namespace datatypes::registry::util;
        constexpr auto sh = iri_prefixes[I].shorthand;
        constexpr auto pr = iri_prefixes[I].prefix;
        return ConstexprString("@prefix ") + ConstexprString<sh.size()+1>(sh) + ConstexprString(": <") + ConstexprString<pr.size()+1>(pr) + ConstexprString("> .\n");
    }

    template<size_t... Idx>
    static consteval auto build(std::index_sequence<Idx...>) {
        return (build_line<Idx>() + ...);
    }
public:
    static consteval std::string_view build() {
        return datatypes::registry::util::ConstexprStringHolder<build(std::make_index_sequence<iri_prefixes.size()>())>::value;
    }
};

bool SerializationState::begin(void *buffer, Cursor *cursor, FlushFunc flush) {
    static constexpr auto d = PrefixDataBuilder::build();
    return write_str(d, buffer, cursor, flush);
}
bool SerializationState::flush(void *buffer, Cursor *cursor, FlushFunc flush) {
    if (!active_predicate.null() || !active_subject.null())
        if (!write_str(" .\n", buffer, cursor, flush))
            return false;
    if (!active_graph.null())
        if (!write_str("}\n", buffer, cursor, flush))
            return false;
    active_predicate = Node::make_null();
    active_subject = Node::make_null();
    active_graph = Node::make_null();
    return true;
}
}