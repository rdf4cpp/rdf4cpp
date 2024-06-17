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

static re2::RE2 build_regex(std::string_view regex, Regex::flag_type flags) {
    auto opt = translate_flags(flags);
    if (!flags.contains(RegexFlag::Multiline) && !flags.contains(RegexFlag::RemoveWhitespace)) {
        return {regex, opt};
    }
    // https://www.w3.org/TR/xpath-functions/#flags
    // re2 does not support x
    // and m needs to be passed as re2 flag (Options::set_one_line is ignored, if Options::posix_syntax == false)
    std::string x{};
    x.reserve(regex.size()+4);
    if (flags.contains(RegexFlag::Multiline)) {
        x.append("(?m)");
    }
    if (flags.contains(RegexFlag::RemoveWhitespace)) {
        uint64_t classes = 0;
        char prev = '\0';
        for (char const c : regex) {
            if (c == '[' && prev != '\\') {
                ++classes;
            } else if (c == ']' && prev != '\\') {
                --classes;
            } else if (classes == 0 && (c == '\t' || c == '\r' || c == '\n' || c == ' ')) {
                continue;
            }
            x.append(1, c);
            prev = c;
        }
    } else {
        x.append(regex);
    }
    return {x, opt};
}

} // namespace detail

Regex::Impl::Impl(std::string_view const regex, Regex::flag_type const flags) : regex{detail::build_regex(regex, flags)}, flags{flags} {
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
