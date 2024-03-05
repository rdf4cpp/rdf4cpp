#include <rdf4cpp/bnode_mngt/reference_backends/generator/RandomIdGenerator.hpp>

#include <array>
#include <algorithm>

namespace rdf4cpp::rdf::bnode_mngt {

namespace generator_detail {

static constexpr std::array<char, 36> bnode_id_valid_chars{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
                                                           'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                                           'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6',
                                                           '7', '8', '9'};

static constexpr size_t generated_id_size = 32;

}  //namespace generator_detail

RandomIdGenerator::RandomIdGenerator() : RandomIdGenerator{std::random_device{}()} {
}

RandomIdGenerator::RandomIdGenerator(uint64_t const seed) : rng{seed},
                                                            dist{0, generator_detail::bnode_id_valid_chars.size() - 1} {
}

size_t RandomIdGenerator::max_generated_id_size() const noexcept {
    return generator_detail::generated_id_size;
}

char *RandomIdGenerator::generate_to_buf(char *const buf) {
    std::unique_lock lock{this->mutex};
    return std::generate_n(buf, generator_detail::generated_id_size, [this]() { return this->next_char(); });
}

char RandomIdGenerator::next_char() {
    return generator_detail::bnode_id_valid_chars[this->dist(this->rng)];
}


}  //namespace rdf4cpp::rdf::bnode_mngt
