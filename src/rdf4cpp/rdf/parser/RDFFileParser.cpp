#include <rdf4cpp/rdf/parser/RDFFileParser.hpp>

namespace rdf4cpp::rdf::parser {
rdf4cpp::rdf::parser::RDFFileParser::RDFFileParser(const std::string &file_path, rdf4cpp::rdf::parser::ParsingFlags flags,
                                                   rdf4cpp::rdf::storage::node::NodeStorage node_storage)
    : file_path_(file_path), flags_(std::move(flags)), node_storage_(std::move(node_storage)) {
}
rdf4cpp::rdf::parser::RDFFileParser::RDFFileParser(std::string &&file_path, rdf4cpp::rdf::parser::ParsingFlags flags,
                                                   rdf4cpp::rdf::storage::node::NodeStorage node_storage)
    : file_path_(std::move(file_path)), flags_(std::move(flags)), node_storage_(std::move(node_storage)) {
}
rdf4cpp::rdf::parser::RDFFileParser::Iterator rdf4cpp::rdf::parser::RDFFileParser::begin() const {
    std::ifstream stream{file_path_};
    if (!stream.is_open())
        return {std::move(stream), {}};
    return {std::move(stream), flags_, node_storage_};
}
std::default_sentinel_t rdf4cpp::rdf::parser::RDFFileParser::end() const noexcept {
    return {};
}

rdf4cpp::rdf::parser::RDFFileParser::Iterator::Iterator(std::ifstream &&stream, IStreamQuadIterator &&iter)
    : stream_(std::move(stream)), iter_(std::move(iter)) {
}
rdf4cpp::rdf::parser::RDFFileParser::Iterator::Iterator(std::ifstream &&stream, rdf4cpp::rdf::parser::ParsingFlags flags,
                                                        rdf4cpp::rdf::storage::node::NodeStorage node_storage)
    : stream_(std::move(stream)), iter_(stream_, flags, {}, node_storage) {
}
rdf4cpp::rdf::parser::RDFFileParser::Iterator::reference rdf4cpp::rdf::parser::RDFFileParser::Iterator::operator*() const noexcept {
    return iter_.operator*();
}
rdf4cpp::rdf::parser::RDFFileParser::Iterator::pointer rdf4cpp::rdf::parser::RDFFileParser::Iterator::operator->() const noexcept {
    return iter_.operator->();
}
rdf4cpp::rdf::parser::RDFFileParser::Iterator &rdf4cpp::rdf::parser::RDFFileParser::Iterator::operator++() {
    ++iter_;
    return *this;
}
bool rdf4cpp::rdf::parser::RDFFileParser::Iterator::operator==(const rdf4cpp::rdf::parser::RDFFileParser::Iterator &other) const noexcept {
    return iter_ == other.iter_;
}
bool operator==(const rdf4cpp::rdf::parser::RDFFileParser::Iterator &iter, std::default_sentinel_t s) noexcept {
    if (!iter.stream_.is_open())
        return true;
    return iter.iter_ == s;
}
bool operator==(std::default_sentinel_t s, const rdf4cpp::rdf::parser::RDFFileParser::Iterator &iter) noexcept {
    return iter == s;
}
}  // namespace rdf4cpp::rdf::parser
