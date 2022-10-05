#ifndef RDF4CPP_PARSER_BYTESOURCE_HPP
#define RDF4CPP_PARSER_BYTESOURCE_HPP

#include <fstream>
#include <concepts>
#include <cstring>

template<typename Src>
concept ByteSource = requires (Src src, char &ch, char *buf, size_t max_bytes) {
                         { src.read_bytes(buf, max_bytes) } -> std::convertible_to<size_t>;
                         { src.is_ok() } -> std::convertible_to<bool>;
                         { src.is_eof() } -> std::convertible_to<bool>;
                     };


struct IStreamByteSource {
private:
    std::reference_wrapper<std::istream> istream;

public:
    inline IStreamByteSource(std::istream &istream)
        : istream{std::ref(istream)} {
    }

    inline size_t read_bytes(char *buf, size_t max_bytes) noexcept {
        if (max_bytes == 1) {
            istream.get().get(*buf);
        } else {
            istream.get().read(buf, static_cast<std::streamsize>(max_bytes));
        }

        return this->istream.get().gcount();
    }

    inline bool is_ok() const noexcept {
        return static_cast<bool>(istream.get());
    }

    inline bool is_eof() const noexcept {
        return istream.get().eof();
    }
};

struct FileByteSource {
private:
    FILE *file;

public:
    inline FileByteSource(char const *path)
        : file{fopen(path, "r")} {

        if (file == nullptr) {
            throw std::runtime_error{strerror(errno)};
        }
    }

    FileByteSource(FileByteSource const &) = delete;

    inline FileByteSource(FileByteSource &&other) : file{other.file} {
        other.file = nullptr;
    }

    inline ~FileByteSource() {
        if (file != nullptr) {
            fclose(file);
        }
    }

    inline size_t read_bytes(char *buf, size_t max_bytes) noexcept {
        return fread(buf, 1, max_bytes, file);
    }

    inline bool is_ok() const noexcept {
        return ferror(file) == 0;
    }

    inline bool is_eof() const noexcept {
        return feof(file) != 0;
    }
};


#endif  //RDF4CPP_PARSER_BYTESOURCE_HPP
