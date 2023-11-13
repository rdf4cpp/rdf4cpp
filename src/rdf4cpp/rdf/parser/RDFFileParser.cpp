#include <rdf4cpp/rdf/parser/RDFFileParser.hpp>

namespace rdf4cpp::rdf::parser {
RDFFileParser::RDFFileParser(const std::string &file_path, flags_type flags, state_type *state)
    : file_path_(file_path), flags_(flags), state_(state) {
}
RDFFileParser::RDFFileParser(std::string &&file_path, flags_type flags, state_type *state)
    : file_path_(std::move(file_path)), flags_(flags), state_(state) {
}
RDFFileParser::Iterator RDFFileParser::begin() const {
    std::ifstream stream{file_path_};
    if (!stream.is_open())
        return {};
    return {std::move(stream), flags_, state_};
}
std::default_sentinel_t RDFFileParser::end() const noexcept {
    return {};
}

RDFFileParser::Iterator::Iterator()
    : stream_(nullptr), iter_(nullptr) {
}
RDFFileParser::Iterator::Iterator(std::ifstream &&stream, flags_type flags, state_type *state)
    : stream_(std::make_unique<std::ifstream>(std::move(stream))),
      iter_(std::make_unique<IStreamQuadIterator>(*stream_, flags, state)) {
}
RDFFileParser::Iterator::reference RDFFileParser::Iterator::operator*() const noexcept {
    return (*iter_).operator*();
}
RDFFileParser::Iterator::pointer RDFFileParser::Iterator::operator->() const noexcept {
    return (*iter_).operator->();
}
RDFFileParser::Iterator &RDFFileParser::Iterator::operator++() {
    ++(*iter_);
    return *this;
}
bool RDFFileParser::Iterator::operator==(const RDFFileParser::Iterator &other) const noexcept {
    return iter_ == other.iter_;
}
bool operator==(const RDFFileParser::Iterator &iter, std::default_sentinel_t s) noexcept {
    if (iter.stream_ == nullptr)
        return true;
    return (*iter.iter_) == s;
}
bool operator==(std::default_sentinel_t s, const RDFFileParser::Iterator &iter) noexcept {
    return iter == s;
}
}  // namespace rdf4cpp::rdf::parser
