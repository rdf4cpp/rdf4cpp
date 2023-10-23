#ifndef RDF4CPP_STREAM_HPP
#define RDF4CPP_STREAM_HPP

#include <cstring>
#include <string_view>
#include <string>
#include <ostream>
#include <istream>

namespace rdf4cpp::rdf {

struct Sink {
    size_t (*write)(void const *buf, size_t elem_size, size_t len, void *stream);
    void (*size_hint)(size_t hint, void *stream);
    int (*error)(void *stream);

    [[nodiscard]] static constexpr Sink make_string_sink() noexcept {
        return Sink{
                .write = [](void const *buf, size_t elem_size, size_t len, void *stream) {
                    reinterpret_cast<std::string *>(stream)->append(reinterpret_cast<char const *>(buf), elem_size * len);
                    return elem_size * len;
                },
                .size_hint = [](size_t hint, void *stream) {
                    reinterpret_cast<std::string *>(stream)->reserve(hint);
                },
                .error = []([[maybe_unused]] void *stream) {
                    return 0;
                }};
    }

    [[nodiscard]] static constexpr Sink make_ostream_sink() noexcept {
        return Sink{
                .write = [](void const *buf, size_t elem_size, size_t len, void *stream_) {
                    auto *stream = reinterpret_cast<std::ostream *>(stream_);
                    return static_cast<size_t>(stream->rdbuf()->sputn(reinterpret_cast<char const *>(buf), static_cast<std::streamsize>(elem_size * len)));
                },
                .size_hint = []([[maybe_unused]] size_t hint, [[maybe_unused]] void *stream) {
                    // ignore
                },
                .error = [](void *stream) {
                    return static_cast<int>(reinterpret_cast<std::ostream *>(stream)->fail());
                }};
    }

    [[nodiscard]] static constexpr Sink make_c_file_sink() noexcept {
        return Sink{
                .write = [](void const *buf, size_t elem_size, size_t len, void *stream) {
                    return fwrite(buf, elem_size, len, reinterpret_cast<FILE *>(stream));
                },
                .size_hint = []([[maybe_unused]] size_t hint, [[maybe_unused]] void *stream) {
                    // ignore
                },
                .error = [](void *stream) {
                    return ferror(reinterpret_cast<FILE *>(stream));
                }};
    }
};

struct Source {
    size_t (*read)(void *buffer, size_t elem_size, size_t len, void *stream);
    size_t (*size_hint)(void *stream);
    int (*error)(void *stream);

    [[nodiscard]] static constexpr Source make_string_view_source() noexcept {
        return Source{
                .read = [](void *buf, size_t elem_size, size_t len, void *stream_) {
                    auto const max_read = elem_size * len;
                    auto *stream = reinterpret_cast<std::string_view *>(stream_);

                    memcpy(buf, stream->data(), max_read);
                    stream->remove_prefix(max_read);
                    return max_read;
                },
                .size_hint = []([[maybe_unused]] void *stream) {
                    return reinterpret_cast<std::string_view *>(stream)->size();
                },
                .error = []([[maybe_unused]] void *stream) {
                    return 0;
                }};
    }

    [[nodiscard]] static constexpr Source make_istream_source() noexcept {
        return Source{
                .read = [](void *buf, size_t elem_size, size_t len, void *stream_) {
                    auto *stream = reinterpret_cast<std::istream *>(stream_);
                    return static_cast<size_t>(stream->rdbuf()->sgetn(reinterpret_cast<char *>(buf), static_cast<std::streamsize>(elem_size * len)));
                },
                .size_hint = [](void *stream_) {
                    auto *stream = reinterpret_cast<std::istream *>(stream_);
                    return static_cast<size_t>(stream->rdbuf()->in_avail());
                },
                .error = [](void *stream) {
                    return static_cast<int>(reinterpret_cast<std::istream *>(stream)->fail());
                }};
    }

    [[nodiscard]] static constexpr Source make_c_file_source() noexcept {
        return Source{
                .read = [](void *buf, size_t elem_size, size_t len, void *stream) {
                    return fread(buf, elem_size, len, reinterpret_cast<FILE *>(stream));
                },
                .size_hint = []([[maybe_unused]] void *stream) {
                    return size_t{0};
                },
                .error = [](void *stream) {
                    return ferror(reinterpret_cast<FILE *>(stream));
                }};
    }
};

} // namespace rdf4cpp::rdf

#endif  // RDF4CPP_STREAM_HPP
