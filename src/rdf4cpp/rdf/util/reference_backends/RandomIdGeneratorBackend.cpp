#include <rdf4cpp/rdf/util/reference_backends/RandomIdGeneratorBackend.hpp>

#include <array>
#include <algorithm>

namespace rdf4cpp::rdf::util {

namespace generator_detail {

static constexpr std::array<char, 36> bnode_id_valid_chars{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
                                                           'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                                           'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6',
                                                           '7', '8', '9'};

static constexpr size_t generated_id_size = 32;

}  //namespace generator_detail

RandomBlankNodeIdGenerator::RandomBlankNodeIdGenerator() : RandomBlankNodeIdGenerator{std::random_device{}()} {
}

RandomBlankNodeIdGenerator::RandomBlankNodeIdGenerator(uint64_t const seed) : rng{seed},
                                                                              dist{0, generator_detail::bnode_id_valid_chars.size() - 1} {
}

size_t RandomBlankNodeIdGenerator::max_generated_id_size() const noexcept {
    return generator_detail::generated_id_size;
}

void RandomBlankNodeIdGenerator::generate_to_string(std::string &buf) {
    std::unique_lock lock{this->mutex};
    std::generate_n(std::back_inserter(buf), generator_detail::generated_id_size, [this]() { return this->next_char(); });
}

char *RandomBlankNodeIdGenerator::generate_to_buf(char *buf) {
    std::unique_lock lock{this->mutex};
    return std::generate_n(buf, generator_detail::generated_id_size, [this]() { return this->next_char(); });
}

char RandomBlankNodeIdGenerator::next_char() {
    return generator_detail::bnode_id_valid_chars[this->dist(this->rng)];
}


}  //namespace rdf4cpp::rdf::util
