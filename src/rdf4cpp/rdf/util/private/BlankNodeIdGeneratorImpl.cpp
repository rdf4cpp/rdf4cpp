#include <rdf4cpp/rdf/util/private/BlankNodeIdGeneratorImpl.hpp>

#include <array>
#include <algorithm>

namespace rdf4cpp::rdf::util {

namespace generator_detail {

static constexpr size_t bnode_id_len = 32;

static constexpr std::array<char, 36> bnode_id_valid_chars{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
                                                           'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                                           'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6',
                                                           '7', '8', '9'};
}  //namespace generator_detail

BlankNodeIdGenerator::Impl::Impl(uint64_t const seed) : rng{seed}, dist{0, generator_detail::bnode_id_valid_chars.size() - 1} {
}

void BlankNodeIdGenerator::Impl::reseed(uint64_t const seed) {
    this->rng.seed(seed);
}

std::string BlankNodeIdGenerator::Impl::generate_id() {
    std::string buf;
    std::generate_n(std::back_inserter(buf), generator_detail::bnode_id_len, [&]() {
        return generator_detail::bnode_id_valid_chars[this->dist(this->rng)];
    });

    return buf;
}

}  //namespace rdf4cpp::rdf::util
