#ifndef RDF4CPP_BUFWRITER_HPP
#define RDF4CPP_BUFWRITER_HPP

#include <array>
#include <cstdio>
#include <cstring>
#include <ostream>
#include <string>
#include <string_view>
#include <system_error>

namespace rdf4cpp::rdf::writer {

/**
 * A cursor into a buffer
 * Describes remaining length (size()) and current write head (data())
 *
 * Memory layout is guaranteed to be identical to:
 * @code
 * struct Cursor {
 *      char *data;
 *      size_t size;
 * };
 * @endcode
 */
struct Cursor {
private:
    char *data_ = nullptr;
    size_t size_ = 0;

public:
    [[nodiscard]] constexpr char *data() const noexcept { return data_; }
    [[nodiscard]] constexpr size_t size() const noexcept { return size_; }

    constexpr void advance(size_t n) noexcept {
        data_ += n;
        size_ -= n;
    }

    constexpr void repoint(char *new_data, size_t new_size) noexcept {
        data_ = new_data;
        size_ = new_size;
    }
};

/**
 * A type that can be used for buffered output.
 * To write bytes to a BufWriter use write_str(std::string_view, BufWriter &w) below.
 *
 * Conceptually, it is three things.
 * Some buffer-like object, a cursor into that buffer and a way to flush the buffer somewhere to make some room.
 *
 * Users of this concept write to the provided buffer in the place where the cursor points.
 * After writing the cursor should be advanced.
 * If the cursor says that there is no more room flush should be called.
 */
template<typename W>
concept BufWriter = requires (W &bw) {
    /**
     * Return type of finalize, typically void
     */
    typename W::Output;

    /**
     * Type of the internal buffer
     */
    typename W::Buffer;

    /**
     * The underlying buffer, the only thing this need to be able to do is be used in flush
     */
    { bw.buffer() } -> std::same_as<typename W::Buffer &>;

    /**
     * The cursor pointing inside of w.buffer()
     */
    { bw.cursor() } -> std::same_as<Cursor &>;

    /**
     * Triggers a final flush, needs to be called after you are done using this BufWriter
     */
    { bw.finalize() } -> std::convertible_to<typename W::Output>;

    /**
     * Flushes bw.buffer() and repoints bw.cursor() to the new free space
     */
    W::flush(&bw.buffer(), bw.cursor());
};

/**
 * Flushes user defined data.
 * the functions task is it to somehow make room in buffer and point the cursor to that new, free space
 *
 * @param buffer buffer to make room in
 * @param cursor a pointer + size pair into buffer (pointing to the free space)
 */
using FlushFunc = void (*)(void *buffer, Cursor &cursor) noexcept;

/**
 * Serializes the given string as is
 *
 * @param str string to write
 * @param buffer pointer to arbitrary buffer structure
 * @param cursor cursor into buffer
 * @param flush function to flush the contents of buffer and update cursor to point to the new free space
 * @return true if serialization was successful, false if a call to flush was not able to make room
 *
 * @see Node::serialize for more details on the signature/usage
 */
inline bool write_str(std::string_view str, void *const buffer, Cursor &cursor, FlushFunc const flush) noexcept {
    while (true) {
        auto const max_write = std::min(str.size(), cursor.size());
        memcpy(cursor.data(), str.data(), max_write);
        cursor.advance(max_write);

        if (max_write == str.size()) [[likely]] {
            break;
        } else {
            if (flush(buffer, cursor); cursor.size() == 0) [[unlikely]] {
                return false;
            }

            str.remove_prefix(max_write);
        }
    }
    return true;
}

/**
 * Serializes a string as is
 * See Node::serialize<> for more details on the signature/usage
 */
template<BufWriter W>
bool write_str(std::string_view str, W &w) {
    return write_str(str, &w.buffer(), w.cursor(), &W::flush);
}

/**
 * (Optional) base class for serializers.
 * Encapsulates a cursor and some buffer type and makes the implementation of flush less error prone.
 *
 * @tparam CRTP derived class that is inheriting from BufWriterBase (https://en.cppreference.com/w/cpp/language/crtp)
 * @tparam Buffer buffer type to be used
 */
template<typename CRTP, typename Buffer>
struct BufWriterBase {
private:
    Buffer buffer_;
    Cursor cursor_;

public:
    template<typename ...BufferArgs>
    explicit constexpr BufWriterBase(BufferArgs &&...buffer_args) : buffer_{std::forward<BufferArgs>(buffer_args)...} {
    }

    [[nodiscard]] constexpr Cursor &cursor() noexcept { return cursor_; }
    [[nodiscard]] constexpr Buffer &buffer() noexcept { return buffer_; }

    static void flush(void *buffer, Cursor &cursor) noexcept {
        CRTP::flush_impl(*reinterpret_cast<typename CRTP::Buffer *>(buffer), cursor);
    }
};

using StringBuffer = std::string;

/**
 * A serializer that serializes to a std::string
 */
struct StringWriter : BufWriterBase<StringWriter, StringBuffer> {
public:
    using Buffer = StringBuffer;
    using Output = std::string;

    explicit StringWriter(size_t cap = 256) noexcept {
        buffer().resize(cap);
        cursor().repoint(buffer().data(), buffer().size());
    }

    [[nodiscard]] Output &finalize() noexcept {
        buffer().resize(static_cast<size_t>(cursor().data() - buffer().data()));
        return buffer();
    }

    static void flush_impl(Buffer &buffer, Cursor &cursor) noexcept {
        auto const bytes_written = cursor.data() - buffer.data();

        buffer.resize(buffer.size() * 2);
        cursor.repoint(buffer.data() + bytes_written,
                       cursor.size() + buffer.size());
    }
};

struct CFileBuffer {
    FILE *file_;
    std::array<char, BUFSIZ> buffer_;

    explicit constexpr CFileBuffer(FILE *file) noexcept : file_{file} {}
};

/**
 * A serializer that serializes to a C FILE
 * It is advisable to set the internal file buffer to nullptr
 * because this serializer has an internal buffer
 */
struct BufCFileWriter : BufWriterBase<BufCFileWriter, CFileBuffer> {
    using Buffer = CFileBuffer;
    using Output = void;

    explicit constexpr BufCFileWriter(FILE *file) noexcept : BufWriterBase<BufCFileWriter, CFileBuffer>{file} {
        cursor().repoint(buffer().buffer_.data(),
                         buffer().buffer_.size());
    }

    void finalize() {
        auto const to_write = static_cast<size_t>(cursor().data() - buffer().buffer_.data());
        if (fwrite(buffer().buffer_.data(), 1, to_write, buffer().file_) < to_write) {
            throw std::system_error{std::error_code{errno, std::system_category()}};
        }
    }

    static void flush_impl(Buffer &buffer, Cursor &cursor) noexcept {
        auto const bytes_written = fwrite(buffer.buffer_.data(), 1, static_cast<size_t>(cursor.data() - buffer.buffer_.data()), buffer.file_);
        cursor.repoint(cursor.data() - bytes_written,
                       cursor.size() + bytes_written);
    }
};

struct OStreamBuffer {
    std::ostream *os_;
    std::array<char, BUFSIZ> buffer_;

    explicit constexpr OStreamBuffer(std::ostream &os) noexcept : os_{&os} {}
};

/**
 * A serializer that serializes to an std::ostream.
 * It is advisable to set the associated stream buffer to nullptr
 * because this serializer has an internal buffer.
 */
struct BufOStreamWriter : BufWriterBase<BufOStreamWriter, OStreamBuffer> {
    using Buffer = OStreamBuffer;
    using Output = void;

    explicit constexpr BufOStreamWriter(std::ostream &os) noexcept : BufWriterBase<BufOStreamWriter, OStreamBuffer>{os} {
        cursor().repoint(buffer().buffer_.data(),
                         buffer().buffer_.size());
    }

    void finalize() {
        if (!buffer().os_->write(buffer().buffer_.data(), static_cast<std::streamsize>(cursor().data() - buffer().buffer_.data()))) {
            throw std::system_error{std::error_code{errno, std::system_category()}};
        }
    }

    static void flush_impl(Buffer &buffer, Cursor &cursor) noexcept {
        if (!buffer.os_->write(buffer.buffer_.data(), static_cast<std::streamsize>(cursor.data() - buffer.buffer_.data()))) {
            return;
        }

        cursor.repoint(buffer.buffer_.data(),
                       buffer.buffer_.size());
    }
};

} // namespace rdf4cpp::rdf::writer

#endif  // RDF4CPP_BUFWRITER_HPP
