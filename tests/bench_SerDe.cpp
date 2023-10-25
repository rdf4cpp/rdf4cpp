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

    CFileSerializer ser{out_file};

    for (auto const &quad : ds) {
        quad.subject().serialize(ser);
        serialize_str(" ", ser);
        quad.predicate().serialize(ser);
        serialize_str(" ", ser);
        quad.object().serialize(ser);
        serialize_str(" .\n", ser);
    }

    ser.finalize();
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
