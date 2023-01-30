#include <rdf4cpp/rdf/util/private/BlankNodeIdGeneratorImpl.hpp>

#include <array>
#include <algorithm>

namespace rdf4cpp::rdf::util {

namespace generator_detail {

static constexpr std::array<char, 36> bnode_id_valid_chars{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
                                                           'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                                           'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6',
                                                           '7', '8', '9'};
}  //namespace generator_detail

BlankNodeIdGenerator::Impl::Impl() : Impl{std::random_device{}()} {
}

BlankNodeIdGenerator::Impl::Impl(uint64_t const seed) : rng{seed},
                                                        dist{0, generator_detail::bnode_id_valid_chars.size() - 1} {
}

void BlankNodeIdGenerator::Impl::generate_to_string(std::string &buf) {
    std::unique_lock lock{this->mutex};
    std::generate_n(std::back_inserter(buf), generated_id_len, [this]() { return this->next_char(); });
}

char *BlankNodeIdGenerator::Impl::generate_to_buf(char *buf) {
    std::unique_lock lock{this->mutex};
    return std::generate_n(buf, generated_id_len, [this]() { return this->next_char(); });
}

char BlankNodeIdGenerator::Impl::next_char() {
    return generator_detail::bnode_id_valid_chars[this->dist(this->rng)];
}

}  //namespace rdf4cpp::rdf::util
