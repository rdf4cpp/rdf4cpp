#ifndef RDF4CPP_EXPECTED_HPP
#define RDF4CPP_EXPECTED_HPP

/**
 * Workaround for buggy detection std::expected detection logic in <nonstd/expected.hpp>
 * (std::expected does not work on clang yet, nonstd/expected.hpp does not honor feature test macro)
 *
 * Additionally the API of std::expected is different from nonstd::expected so it is not a drop in replacement.
 */

#define nsel_CONFIG_SELECT_EXPECTED 1 // do not attempt to use std::expected
#include <nonstd/expected.hpp>

#endif // RDF4CPP_EXPECTED_HPP
