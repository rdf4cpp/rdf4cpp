#ifndef RDF4CPP_SERIALIZE_HPP
#define RDF4CPP_SERIALIZE_HPP

#include <array>
#include <cstdio>
#include <cstring>
#include <ostream>
#include <string>
#include <string_view>
#include <system_error>

namespace rdf4cpp::rdf {

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

template<typename T>
concept Serializer = requires (T &ser) {
    typename T::Output;
    typename T::Buffer;

    { ser.buffer() } -> std::same_as<typename T::Buffer &>;
    { ser.cursor() } -> std::same_as<Cursor &>;
    { ser.finalize() } -> std::convertible_to<typename T::Output>;
    T::flush(&ser.buffer(), ser.cursor());
};

/**
 * Flushes user defined data
 */
using FlushFunc = void (*)(void *buffer, Cursor &cursor) noexcept;

/**
 * Serializes the given string as is
 * See Node::serialize for more details on the signature/usage
 */
inline bool serialize_str(std::string_view str, void *const buffer, Cursor &cursor, FlushFunc const flush) noexcept {
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
template<Serializer Ser>
bool serialize_str(std::string_view str, Ser &ser) {
    return serialize_str(str, &ser.buffer(), ser.cursor(), &Ser::flush);
}

/**
 * (Optional) base class for serializers.
 * Encapsulates a cursor and some buffer type and makes the implementation of flush less error prone.
 */
template<typename CRTP, typename Buffer>
struct SerializerBase {
private:
    Buffer buffer_;
    Cursor cursor_;

public:
    template<typename ...BufferArgs>
    explicit constexpr SerializerBase(BufferArgs &&...buffer_args) : buffer_{std::forward<BufferArgs>(buffer_args)...} {
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
struct StringSerializer : SerializerBase<StringSerializer, StringBuffer> {
public:
    using Buffer = StringBuffer;
    using Output = std::string;

    explicit StringSerializer(size_t cap = 256) noexcept {
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
struct CFileSerializer : SerializerBase<CFileSerializer, CFileBuffer> {
    using Buffer = CFileBuffer;
    using Output = void;

    explicit constexpr CFileSerializer(FILE *file) noexcept : SerializerBase<CFileSerializer, CFileBuffer>{file} {
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
struct OStreamSerializer : SerializerBase<OStreamSerializer, OStreamBuffer> {
    using Buffer = OStreamBuffer;
    using Output = void;

    explicit constexpr OStreamSerializer(std::ostream &os) noexcept : SerializerBase<OStreamSerializer, OStreamBuffer>{os} {
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

} // namespace rdf4cpp::rdf

#endif // RDF4CPP_SERIALIZE_HPP
