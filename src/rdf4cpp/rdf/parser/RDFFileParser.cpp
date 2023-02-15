#include "RDFFileParser.hpp"

rdf4cpp::rdf::parser::RDFFileParser::RDFFileParser(const std::string &filePath, rdf4cpp::rdf::parser::ParsingFlags flags,
                                                   rdf4cpp::rdf::storage::node::NodeStorage node_storage)
    : filePath_(filePath), flags_(std::move(flags)), node_storage_(std::move(node_storage)) {
}
rdf4cpp::rdf::parser::RDFFileParser::RDFFileParser(std::string &&filePath, rdf4cpp::rdf::parser::ParsingFlags flags,
                                                   rdf4cpp::rdf::storage::node::NodeStorage node_storage)
    : filePath_(std::move(filePath)), flags_(std::move(flags)), node_storage_(std::move(node_storage)) {
}
rdf4cpp::rdf::parser::RDFFileParser::Iterator rdf4cpp::rdf::parser::RDFFileParser::begin() {
    std::ifstream stream{filePath_};
    if (!stream.is_open())
        return {std::move(stream), {}};
    return {std::move(stream), flags_, node_storage_};
}
std::default_sentinel_t rdf4cpp::rdf::parser::RDFFileParser::end() {
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
bool rdf4cpp::rdf::parser::operator==(const rdf4cpp::rdf::parser::RDFFileParser::Iterator &iter, std::default_sentinel_t s) noexcept {
    if (!iter.stream_.is_open())
        return true;
    return iter.iter_ == s;
}
bool rdf4cpp::rdf::parser::operator==(std::default_sentinel_t s, const rdf4cpp::rdf::parser::RDFFileParser::Iterator &iter) noexcept {
    return iter == s;
}
