#include <rdf4cpp/rdf/parser/RDFFileParser.hpp>

namespace rdf4cpp::rdf::parser {
RDFFileParser::RDFFileParser(const std::string &file_path, ParsingFlags flags,
                                                   rdf4cpp::rdf::storage::node::NodeStorage node_storage)
    : file_path_(file_path), flags_(flags), node_storage_(std::move(node_storage)) {
}
RDFFileParser::RDFFileParser(std::string &&file_path, ParsingFlags flags,
                                                   rdf4cpp::rdf::storage::node::NodeStorage node_storage)
    : file_path_(std::move(file_path)), flags_(flags), node_storage_(std::move(node_storage)) {
}
RDFFileParser::iterator RDFFileParser::begin() const {
    FILE *stream = fopen(file_path_.c_str(), "r");
    if (stream == nullptr) {
        throw std::system_error{errno, std::system_category()};
    }
    return {stream, flags_, node_storage_};
}
std::default_sentinel_t RDFFileParser::end() const noexcept {
    return {};
}

RDFFileParser::iterator::iterator()
    : stream_(nullptr), iter_(nullptr) {
}
RDFFileParser::iterator::iterator(FILE *stream, ParsingFlags flags,
                                  const rdf4cpp::rdf::storage::node::NodeStorage &node_storage)
    : stream_(stream),
      iter_(std::make_unique<IStreamQuadIterator>(stream_, Source::make_c_file_source(), flags, IStreamQuadIterator::prefix_storage_type{}, node_storage)) {
}
RDFFileParser::iterator::~iterator() noexcept {
    fclose(stream_);
}
RDFFileParser::iterator::reference RDFFileParser::iterator::operator*() const noexcept {
    return (*iter_).operator*();
}
RDFFileParser::iterator::pointer RDFFileParser::iterator::operator->() const noexcept {
    return (*iter_).operator->();
}
RDFFileParser::iterator &RDFFileParser::iterator::operator++() {
    ++(*iter_);
    return *this;
}
bool RDFFileParser::iterator::operator==(const RDFFileParser::iterator &other) const noexcept {
    return iter_ == other.iter_;
}
bool operator==(const RDFFileParser::iterator &iter, std::default_sentinel_t s) noexcept {
    return (*iter.iter_) == s;
}
bool operator==(std::default_sentinel_t s, const RDFFileParser::iterator &iter) noexcept {
    return iter == s;
}
}  // namespace rdf4cpp::rdf::parser
