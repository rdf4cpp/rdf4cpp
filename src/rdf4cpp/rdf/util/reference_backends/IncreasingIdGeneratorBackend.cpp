#include "IncreasingIdGeneratorBackend.hpp"

#include <cassert>
#include <charconv>
#include <limits>


namespace rdf4cpp::rdf::util {

IncreasingIdGeneratorBackend::IncreasingIdGeneratorBackend() noexcept : prefix{},
                                                                        counter{0} {
}

IncreasingIdGeneratorBackend::IncreasingIdGeneratorBackend(size_t const initial_value, std::string prefix) : prefix{std::move(prefix)},
                                                                                                             counter{initial_value} {
}

size_t IncreasingIdGeneratorBackend::max_generated_id_size() const noexcept {
    return std::numeric_limits<size_t>::digits10;
}

void IncreasingIdGeneratorBackend::generate_to_string(std::string &buf) {
    std::copy(this->prefix.begin(), this->prefix.end(), std::back_inserter(buf));

    auto *begin = buf.data() + buf.size();
    buf.resize(buf.size() + this->max_generated_id_size());
    auto *end = buf.data() + buf.size();

    std::to_chars_result const res = std::to_chars(begin, end, this->counter.fetch_add(1, std::memory_order_relaxed));
    assert(res.ec == std::errc{});

    buf.resize(res.ptr - buf.data());
}

char *IncreasingIdGeneratorBackend::generate_to_buf(char *buf) {
    buf = std::copy(this->prefix.begin(), this->prefix.end(), buf);

    std::to_chars_result const res = std::to_chars(buf, buf + this->max_generated_id_size(), this->counter.fetch_add(1, std::memory_order_relaxed));
    assert(res.ec == std::errc{});
    return res.ptr;
}

}  //namespace rdf4cpp::rdf::util
