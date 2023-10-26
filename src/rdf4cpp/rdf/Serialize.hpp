#ifndef RDF4CPP_SERIALIZE_HPP
#define RDF4CPP_SERIALIZE_HPP

#include <array>
#include <cstring>
#include <ostream>
#include <string>
#include <utility>
#include <system_error>
#include <string_view>

namespace rdf4cpp::rdf {

template<typename T>
concept Serializer = requires (T &ser, void *voided_self) {
    typename T::Output;

    { ser.buf() } -> std::same_as<char *&>;
    { ser.buf_size() } -> std::same_as<size_t &>;
    { ser.finalize() } -> std::convertible_to<typename T::Output>;
    T::flush(voided_self);
};

/**
 * Flushes user defined data
 */
using FlushFunc = size_t (*)(void *data) noexcept;

/**
 * Serializes a string
 */
inline bool serialize_str(std::string_view str, char **buf, size_t *buf_size, FlushFunc flush, void *data) noexcept {
    while (true) {
        auto const max_write = std::min(str.size(), *buf_size);
        memcpy(*buf, str.data(), max_write);
        *buf += max_write;
        *buf_size -= max_write;

        if (max_write == str.size()) [[likely]] {
            break;
        } else {
            str.remove_prefix(max_write);
            if (flush(data); *buf_size == 0) [[unlikely]] {
                return false;
            }
        }
    }
    return true;
}

/**
 * Serializes a string using a serializer
 * @param str string
 * @param ser serializer
 * @return true on success, false otherwise
 */
template<Serializer Ser>
bool serialize_str(std::string_view str, Ser &ser) {
    return serialize_str(str, &ser.buf(), &ser.buf_size(), &Ser::flush, &ser);
}

template<typename CRTP>
struct SerializerBase {
private:
    char *buf_ = nullptr;
    size_t buf_size_ = 0;

public:
    [[nodiscard]] char *&buf() noexcept { return buf_; }
    [[nodiscard]] size_t &buf_size() noexcept { return buf_size_; }

    static void flush(void *self) noexcept {
        return reinterpret_cast<CRTP *>(self)->flush_impl();
    }
};

/**
 * A serializer that serializes to a std::string
 */
struct StringSerializer : SerializerBase<StringSerializer> {
private:
    std::string buffer_;

public:
    using Output = std::string;

    explicit StringSerializer(size_t cap = 256) noexcept {
        buffer_.resize(cap);
        buf() = buffer_.data();
        buf_size() = buffer_.size();
    }

    Output &finalize() noexcept {
        buffer_.resize(static_cast<size_t>(buf() - buffer_.data()));
        return buffer_;
    }

    void flush_impl() noexcept {
        auto const bytes_written = buf() - buffer_.data();

        buffer_.resize(buffer_.size() * 2);
        buf() = buffer_.data() + bytes_written;
        buf_size() += buffer_.size();
    }
};

/**
 * A serializer that serializes to a C FILE
 * It is advisable to set the internal file buffer to nullptr
 * because this serializer has an internal buffer
 */
struct CFileSerializer : SerializerBase<CFileSerializer> {
private:
    FILE *file_;
    std::array<char, BUFSIZ> buffer_;

public:
    using Output = void;

    explicit constexpr CFileSerializer(FILE *file) noexcept : file_{file} {
        buf() = buffer_.data();
        buf_size() = buffer_.size();
    }

    void finalize() {
        auto to_write = static_cast<size_t>(buf() - buffer_.data());
        if (fwrite(buffer_.data(), 1, to_write, file_) < to_write) {
            throw std::system_error{std::error_code{errno, std::system_category()}};
        }
    }

    void flush_impl() noexcept {
        auto const bytes_written = fwrite(buffer_.data(), 1, static_cast<size_t>(buf() - buffer_.data()), file_);
        buf() -= bytes_written;
        buf_size() += bytes_written;
    }
};

/**
 * A serializer that serializes to an std::ostream.
 * It is advisable to set the associated stream buffer to nullptr
 * because this serializer has an internal buffer.
 */
struct OStreamSerializer : SerializerBase<CFileSerializer> {
private:
    std::ostream *os_;
    std::array<char, BUFSIZ> buffer_;

public:
    using Output = void;

    explicit constexpr OStreamSerializer(std::ostream &os) noexcept : os_{&os} {
        buf() = buffer_.data();
        buf_size() = buffer_.size();
    }

    void finalize() {
        if (!os_->write(buffer_.data(), static_cast<size_t>(buf() - buffer_.data()))) {
            throw std::system_error{std::error_code{errno, std::system_category()}};
        }
    }

    void flush_impl() noexcept {
        auto const bytes_written = static_cast<size_t>(static_cast<bool>(
                os_->write(buffer_.data(), static_cast<std::streamsize>(buf() - buffer_.data()))));

        buf() -= bytes_written;
        buf_size() += bytes_written;
    }
};

} // namespace rdf4cpp::rdf

#endif // RDF4CPP_SERIALIZE_HPP
