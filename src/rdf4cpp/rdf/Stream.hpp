#ifndef RDF4CPP_STREAM_HPP
#define RDF4CPP_STREAM_HPP

#include <cassert>
#include <cstring>
#include <string_view>
#include <string>
#include <ostream>
#include <istream>

namespace rdf4cpp::rdf {

struct Sink {
    /**
     * Identical semantics to fwrite.
     *
     * @param buf buffer to read from
     * @param elem_size sizeof elements in buffer, guaranteed to be always set to 1
     * @param count the number of elements in buffer
     * @param stream the stream to write into
     * @return number of elements (=bytes) successfully written. Returns value < count if an error occurred.
     */
    size_t (*write)(void const *buf, size_t elem_size, size_t count, void *stream) noexcept;

    [[nodiscard]] static constexpr Sink make_string_sink() noexcept {
        return Sink{
                .write = [](void const *buf, [[maybe_unused]] size_t elem_size, size_t count, void *stream) noexcept {
                    assert(elem_size == 1);
                    reinterpret_cast<std::string *>(stream)->append(reinterpret_cast<char const *>(buf), count);
                    return count;
                }};
    }

    [[nodiscard]] static constexpr Sink make_ostream_sink() noexcept {
        return Sink{
                .write = [](void const *buf, [[maybe_unused]] size_t elem_size, size_t count, void *stream_) noexcept -> size_t {
                    assert(elem_size == 1);

                    auto *stream = reinterpret_cast<std::ostream *>(stream_);
                    if (stream->write(reinterpret_cast<char const *>(buf), static_cast<std::streamsize>(count))) {
                        return count;
                    }

                    return 0;
                }};
    }

    [[nodiscard]] static constexpr Sink make_c_file_sink() noexcept {
        return Sink{
                .write = [](void const *buf, size_t elem_size, size_t len, void *stream) noexcept {
                    return fwrite(buf, elem_size, len, reinterpret_cast<FILE *>(stream));
                }};
    }
};

struct Source {
    /**
     * Identical semantics to fread
     *
     * @param buffer buffer to read into
     * @param elem_size sizeof elements in buffer, guaranteed to be always set to 1
     * @param count number of elements (=bytes) in buffer
     * @param stream stream to read from
     * @return number of bytes read
     */
    size_t (*read)(void *buffer, size_t elem_size, size_t count, void *stream) noexcept;

    /**
     * Identical semantics to ferror
     *
     * @param stream stream to check for error
     * @return zero on ok, non-zero on error
     */
    int (*error)(void *stream) noexcept;

    [[nodiscard]] static constexpr Source make_string_view_source() noexcept {
        return Source{
                .read = [](void *buf, [[maybe_unused]] size_t elem_size, size_t count, void *stream_) noexcept {
                    assert(elem_size == 1);

                    auto *stream = reinterpret_cast<std::string_view *>(stream_);
                    memcpy(buf, stream->data(), count);
                    stream->remove_prefix(count);

                    return count;
                },
                .error = []([[maybe_unused]] void *stream) noexcept {
                    return 0;
                }};
    }

    [[nodiscard]] static constexpr Source make_istream_source() noexcept {
        return Source{
                .read = [](void *buf, [[maybe_unused]] size_t elem_size, size_t count, void *stream_) noexcept {
                    assert(elem_size == 1);

                    auto *stream = reinterpret_cast<std::istream *>(stream_);
                    stream->read(reinterpret_cast<char *>(buf), static_cast<std::streamsize>(count));

                    return static_cast<size_t>(stream->gcount());
                },
                .error = [](void *stream) noexcept {
                    return static_cast<int>(reinterpret_cast<std::istream *>(stream)->fail());
                }};
    }

    [[nodiscard]] static constexpr Source make_c_file_source() noexcept {
        return Source{
                .read = [](void *buf, size_t elem_size, size_t len, void *stream) noexcept {
                    return fread(buf, elem_size, len, reinterpret_cast<FILE *>(stream));
                },
                .error = [](void *stream) noexcept {
                    return ferror(reinterpret_cast<FILE *>(stream));
                }};
    }
};

} // namespace rdf4cpp::rdf

#endif  // RDF4CPP_STREAM_HPP
