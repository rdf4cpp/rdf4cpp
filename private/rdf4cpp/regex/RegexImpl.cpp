#include "RegexImpl.hpp"

namespace rdf4cpp::regex {

namespace detail {

/**
 * Translates the regex flags of rdf4cpp's public interface to RE2 Options.
 *
 * @param flags the rdf4cpp flags to translate
 * @return the translated RE2 Options
 */
static re2::RE2::Options translate_flags(Regex::flag_type const flags) {
    re2::RE2::Options o;
    o.set_log_errors(false);
    o.set_dot_nl(flags.contains(RegexFlag::DotAll));
    o.set_case_sensitive(!flags.contains(RegexFlag::CaseInsensitive));
    o.set_literal(flags.contains(RegexFlag::Literal));

    return o;
}

} // namespace detail

Regex::Impl::Impl(std::string_view const regex, Regex::flag_type const flags) : regex{regex, detail::translate_flags(flags)}, flags{flags} {
    if (!this->regex.ok()) {
        throw RegexError{"Failed to compile regex: " + this->regex.error()};
    }
}

bool Regex::Impl::regex_match(std::string_view const str) const noexcept {
    return RE2::FullMatch(str, this->regex);
}

bool Regex::Impl::regex_search(std::string_view const str) const noexcept {
    return RE2::PartialMatch(str, this->regex);
}

}  //namespace rdf4cpp::regex
