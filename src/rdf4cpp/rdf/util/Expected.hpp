/**
 * Workaround for buggy detection logic for std::expected in <nonstd/expected.hpp>
 */

#if __cpp_lib_expected >= 202202L

#include <expected>
namespace nonstd {
    using std::expected;
} // namepace nonstd

#else

#define nsel_CONFIG_SELECT_EXPECTED 1 // do not attempt to use std::expected
#include <nonstd/expected.hpp>

#endif
