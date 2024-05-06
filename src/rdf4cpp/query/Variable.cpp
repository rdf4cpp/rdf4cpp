#include "Variable.hpp"

#include <rdf4cpp/writer/TryWrite.hpp>
#include <rdf4cpp/util/CharMatcher.hpp>
#include <rdf4cpp/ParsingError.hpp>
#include <uni_algo/all.h>

namespace rdf4cpp::query {
Variable::Variable() noexcept : Node{storage::identifier::NodeBackendHandle{{}, storage::identifier::RDFNodeType::Variable, {}}} {
}

Variable::Variable(std::string_view name, bool anonymous, storage::DynNodeStoragePtr node_storage)
    : Variable{make_unchecked(check_var_name(name), anonymous, node_storage)} {}

Variable::Variable(storage::identifier::NodeBackendHandle handle) noexcept : Node{handle} {}

Variable Variable::make_named(std::string_view name, storage::DynNodeStoragePtr node_storage) {
    return Variable{name, false, node_storage};
}

Variable Variable::make_anonymous(std::string_view name, storage::DynNodeStoragePtr node_storage) {
    return Variable{name, true, node_storage};
}

Variable Variable::make_unchecked(std::string_view name, bool anonymous, storage::DynNodeStoragePtr node_storage) {
    return Variable{storage::identifier::NodeBackendHandle{node_storage.find_or_make_id(storage::view::VariableBackendView{.name = name, .is_anonymous = anonymous}),
                    storage::identifier::RDFNodeType::Variable,
                    node_storage}};
}

Variable Variable::to_node_storage(storage::DynNodeStoragePtr node_storage) const {
    if (handle_.storage() == node_storage) {
        return *this;
    }

    auto const node_id = node_storage.find_or_make_id(handle_.variable_backend());
    return Variable{storage::identifier::NodeBackendHandle{node_id, storage::identifier::RDFNodeType::Variable, node_storage}};
}

Variable Variable::try_get_in_node_storage(storage::DynNodeStoragePtr node_storage) const noexcept {
    if (handle_.storage() == node_storage) {
        return *this;
    }

    auto const node_id = node_storage.find_id(handle_.variable_backend());
    if (node_id == storage::identifier::NodeID{}) {
        return Variable{};
    }

    return Variable{storage::identifier::NodeBackendHandle{node_id, storage::identifier::RDFNodeType::Variable, node_storage}};
}

Variable Variable::find(std::string_view name, bool anonymous, storage::DynNodeStoragePtr node_storage) noexcept {
    auto nid = node_storage.find_id(storage::view::VariableBackendView{.name = name, .is_anonymous = anonymous});

    if (nid.null())
        return Variable{};

    return Variable{storage::identifier::NodeBackendHandle{nid, storage::identifier::RDFNodeType::Variable, node_storage}};
}
Variable Variable::find_named(std::string_view name, storage::DynNodeStoragePtr node_storage) noexcept {
    return find(name, false, node_storage);
}
Variable Variable::find_anonymous(std::string_view name, storage::DynNodeStoragePtr node_storage) noexcept {
    return find(name, true, node_storage);
}

bool Variable::is_anonymous() const {
    // TODO: encode is_anonymous into variable ID
    return this->handle_.variable_backend().is_anonymous;
}
std::string_view Variable::name() const {
    return this->handle_.variable_backend().name;
}

bool Variable::serialize(writer::BufWriterParts const writer) const noexcept {
    if (null()) {
        return rdf4cpp::writer::write_str("null", writer);
    }

    auto const backend = handle_.variable_backend();

    if (backend.is_anonymous) {
        RDF4CPP_DETAIL_TRY_WRITE_STR("_:");
    } else {
        RDF4CPP_DETAIL_TRY_WRITE_STR("?");
    }

    RDF4CPP_DETAIL_TRY_WRITE_STR(backend.name);
    return true;
}

Variable::operator std::string() const {
    return writer::StringWriter::oneshot([this](auto &w) noexcept {
        return this->serialize(w);
    });
}

bool Variable::is_literal() const { return false; }
bool Variable::is_variable() const { return true; }
bool Variable::is_blank_node() const { return false; }
bool Variable::is_iri() const { return false; }

std::ostream &operator<<(std::ostream &os, Variable const &variable) {
    writer::BufOStreamWriter w{os};
    variable.serialize(w);
    w.finalize();

    return os;
}

std::string_view Variable::check_var_name(std::string_view n) {
    using namespace util::char_matcher_detail;
    static constexpr auto first_matcher = ASCIINumMatcher{} | PNCharsBaseMatcher;
    auto r = n | una::views::utf8;
    auto it = r.begin();
    if (it == r.end()) {
        throw ParsingError("invalid blank node label (empty string)");
    }
    if (!first_matcher.match(*it)) {
        throw ParsingError(std::format("invalid blank node label {}", n));
    }
    ++it;
    while (it != r.end()) {
        if (!PNCharsMatcher.match(*it))
        {
            throw ParsingError(std::format("invalid blank node label {}", n));
        }
        ++it;
    }
    return n;
}

}  // namespace rdf4cpp::query

auto std::formatter<rdf4cpp::query::Variable>::format(rdf4cpp::query::Variable n, format_context &ctx) const -> decltype(ctx.out()) {
    rdf4cpp::writer::BufOutputIteratorWriter w{ctx.out()};
    n.serialize(w);
    w.finalize();
    return w.buffer().iter;
}
