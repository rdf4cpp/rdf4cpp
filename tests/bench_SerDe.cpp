#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>

#include <filesystem>
#include <rdf4cpp/rdf.hpp>

void download_swdf(std::filesystem::path const &base) {
    auto curl_cmd = std::format("wget -P '{}' https://hobbitdata.informatik.uni-leipzig.de/ISWC2020_Tentris/swdf.zip", base.c_str());
    std::system(curl_cmd.c_str());

    auto const swdf_path = base / "swdf.zip";
    auto unzip_cmd = std::format("unzip -d '{}' '{}'", base.c_str(), swdf_path.c_str());
    std::system(unzip_cmd.c_str());
}

using namespace rdf4cpp::rdf;

void deserialize(std::filesystem::path const &in_path, Dataset &ds, storage::node::NodeStorage &node_storage) {
    FILE *in_file = fopen(in_path.c_str(), "r");
    if (in_file == nullptr) {
        throw std::system_error{std::error_code{errno, std::system_category()}};
    }
    setbuf(in_file, nullptr);

    parser::IStreamQuadIterator qit{in_file,
                                    reinterpret_cast<parser::ReadFunc>(&fread),
                                    reinterpret_cast<parser::ErrorFunc>(&ferror),
                                    parser::ParsingFlags::none(),
                                    {},
                                    node_storage};

    for (; qit != std::default_sentinel; ++qit) {
        if (qit->has_value()) {
            ds.add(**qit);
        }
    }

    fclose(in_file);
}

void serialize(std::filesystem::path const &out_path, Dataset const &ds) {
    FILE *out_file = fopen(out_path.c_str(), "w");
    if (out_file == nullptr) {
        throw std::system_error{std::error_code{errno, std::system_category()}};
    }
    setbuf(out_file, nullptr);

    struct Buffer {
        FILE *out_file;
        std::array<char, BUFSIZ> buffer;

        constexpr Buffer(FILE *file) noexcept : out_file{file} {
        }

        static void flush(void *self_, char **buf, size_t *buf_size) noexcept {
            auto *self = reinterpret_cast<Buffer *>(self_);

            auto const bytes_written = fwrite(self->buffer.data(), 1, *buf - self->buffer.data(), self->out_file);
            *buf -= bytes_written;
            *buf_size += bytes_written;
        }
    };

    Buffer buffer{out_file};

    auto write = [&, data = &buffer, flush = &Buffer::flush](char **buf, size_t *buf_size, std::string_view str2) {
        TRY_WRITE(str2);
        return true;
    };

    char *buf = buffer.buffer.data();
    size_t buf_size = buffer.buffer.size();
    for (auto const &quad : ds) {
        quad.subject().serialize(&buf, &buf_size, &Buffer::flush, &buffer);
        write(&buf, &buf_size, " ");
        quad.predicate().serialize(&buf, &buf_size, &Buffer::flush, &buffer);
        write(&buf, &buf_size, " ");
        quad.object().serialize(&buf, &buf_size, &Buffer::flush, &buffer);
        write(&buf, &buf_size, " .\n");
    }

    fwrite(buffer.buffer.data(), 1, buf - buffer.buffer.data(), buffer.out_file);
    fclose(out_file);
}

int main() {
    std::filesystem::path const base = std::format("/tmp/rdf4cpp-bench-{}", std::random_device{}());
    std::filesystem::create_directory(base);

    std::filesystem::path const in_path = base / "swdf.nt";
    std::filesystem::path const out_path = "/dev/null";

    download_swdf(base);

    auto ser_ns = storage::node::NodeStorage::new_instance();
    Dataset ser_ds{ser_ns};
    deserialize(in_path, ser_ds, ser_ns); // prepare dataset for serialization bench

    ankerl::nanobench::Bench{}
            .unit("SWDF")
            .run("deserialization", [&in_path]() {
                auto ns = storage::node::NodeStorage::new_instance();
                Dataset ds{ns};
                deserialize(in_path, ds, ns);
            })
            .run("serialization", [&out_path, &ser_ds]() {
                serialize(out_path, ser_ds);
            });

    std::error_code ec;
    std::filesystem::remove_all(base, ec);
    // ignore ec
}
