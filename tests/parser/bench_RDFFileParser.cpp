#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>

#include <curl/curl.h>

#include <rdf4cpp/rdf.hpp>

#include <chrono>
#include <filesystem>

void read_file_from_url(std::string const &url, std::filesystem::path const &out_path) {
    FILE *out_f = fopen(out_path.c_str(), "w");
    if (out_f == nullptr) {
        throw std::runtime_error{std::format("Could not open file in /tmp for writing: {}", strerror(errno))};
    }

    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, out_f);

    auto res = curl_easy_perform(curl);

    curl_easy_cleanup(curl);
    fclose(out_f);

    if (res != CURLE_OK) {
        throw std::runtime_error{std::format("CURL failed: {}", curl_easy_strerror(res))};
    }
}

using namespace rdf4cpp::rdf;

void deserialize(FILE *in_file, Dataset &ds, storage::node::NodeStorage &node_storage) {
    fseek(in_file, 0, SEEK_SET);

    parser::IStreamQuadIterator qit{in_file, Source::make_c_file_source(),
                                    parser::ParsingFlags::none(),
                                    {},
                                    node_storage};

    for (; qit != std::default_sentinel; ++qit) {
        if (qit->has_value()) {
            ds.add(**qit);
        }
    }
}

int main() {
    std::filesystem::path const in_path = std::format("/tmp/rdf4cpp-bench-swdf-in-{}.nt", std::random_device{}());
    std::filesystem::path const out_path = "/dev/null";

    read_file_from_url("https://hobbitdata.informatik.uni-leipzig.de/ISWC2020_Tentris/swdf.zip", in_path);

    char in_buf[BUFSIZ];
    char out_buf[BUFSIZ];

    FILE *in_file = fopen(in_path.c_str(), "r");
    if (in_file == nullptr) {
        throw std::system_error{std::error_code{errno, std::system_category()}};
    }

    setbuf(in_file, in_buf);

    FILE *out_file = fopen(out_path.c_str(), "w");
    if (out_file == nullptr) {
        throw std::system_error{std::error_code{errno, std::system_category()}};
    }

    setbuf(out_file, out_buf);

    Dataset ser_ds;
    auto ser_ns = storage::node::NodeStorage::new_instance();
    deserialize(in_file, ser_ds, ser_ns); // prepare dataset for serialization bench

    static constexpr auto ser = Sink::make_c_file_sink();

    ankerl::nanobench::Bench{}
            .unit("swdf")
            .minEpochIterations(30000)
            .epochs(1000)
            /*.run("deserialization", [in_file]() {
                Dataset ds;
                auto ns = storage::node::NodeStorage::new_instance();
                deserialize(in_file, ds, ns);
            })*/
            .run("serialization", [out_file, &ser_ds]() {
                for (auto const &quad : ser_ds) {
                    quad.subject().serialize(out_file, ser);
                    ser.write(" ", 1, 1, out_file);
                    quad.predicate().serialize(out_file, ser);
                    ser.write(" ", 1, 1, out_file);
                    quad.object().serialize(out_file, ser);
                    ser.write(" .\n", 1, 3, out_file);
                }
            });

    fclose(out_file);
    fclose(in_file);

    std::error_code ec;
    std::filesystem::remove(in_path, ec);
    std::filesystem::remove(out_path, ec);
    // ignore ec
}
