#include <rdf4cpp/rdf/bnode_mngt/reference_backends/generator/IncreasingIdGenerator.hpp>

#include <algorithm>
#include <cassert>
#include <charconv>
#include <limits>

namespace rdf4cpp::rdf::bnode_mngt {

namespace generator_detail {
static constexpr size_t max_generated_id_size = std::numeric_limits<size_t>::digits10;
} //namespace generator_detail

IncreasingIdGenerator::IncreasingIdGenerator(std::string prefix, size_t const initial_value) noexcept : prefix{std::move(prefix)},
                                                                                                        counter{initial_value} {
}

size_t IncreasingIdGenerator::max_generated_id_size() const noexcept {
    return generator_detail::max_generated_id_size + this->prefix.size();
}

char *IncreasingIdGenerator::generate_to_buf(char *buf) {
    buf = std::copy(this->prefix.begin(), this->prefix.end(), buf);

    std::to_chars_result const res = std::to_chars(buf, buf + generator_detail::max_generated_id_size, this->counter.fetch_add(1, std::memory_order_relaxed));
    assert(res.ec == std::errc{});
    return res.ptr;
}

}  //namespace rdf4cpp::rdf::bnode_mngt
