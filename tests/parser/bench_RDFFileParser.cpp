#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <rdf4cpp/rdf.hpp>

#include <chrono>
#include <filesystem>

TEST_CASE("Benchmark") {
    using namespace rdf4cpp::rdf;

    std::filesystem::path const in_path = "/home/liss/Dokumente/dbpedia_2015-10_en_wo-comments_c.nt";
    std::filesystem::path const out_path = "/home/liss/Dokumente/test.ser.nt";

    Dataset ds;

    {
        auto const start_t = std::chrono::steady_clock::now();

        parser::RDFFileParser p{in_path};
        for (auto const &quad : p) {
            if (quad.has_value()) {
                ds.add(*quad);
            } else {
                std::cerr << quad.error() << std::endl;
            }
        }

        auto const end_t = std::chrono::steady_clock::now();

        auto const duration = end_t - start_t;
        auto const duration_s = std::chrono::duration_cast<std::chrono::duration<double>>(duration).count();
        auto const file_size = std::filesystem::file_size(in_path);
        auto const file_size_mb = static_cast<double>(file_size) / 1000.0 / 1000.0;

        std::cout << "Achieved: " << (file_size_mb / duration_s) << " MB/s deserializing" << std::endl;
    }

    {
        auto const start_t = std::chrono::steady_clock::now();

        FILE *out_file = fopen(out_path.c_str(), "w");
        assert(out_file != nullptr);

        auto const ser = Sink::make_c_file_sink();

        for (auto const &quad : ds) {
            quad.subject().serialize(out_file, ser);
            ser.write(" ", 1, 1, out_file);
            quad.predicate().serialize(out_file, ser);
            ser.write(" ", 1, 1, out_file);
            quad.object().serialize(out_file, ser);
            ser.write(" .\n", 1, 3, out_file);
        }

        fclose(out_file);

        auto const end_t = std::chrono::steady_clock::now();

        auto const duration = end_t - start_t;
        auto const duration_s = std::chrono::duration_cast<std::chrono::duration<double>>(duration).count();
        auto const file_size = std::filesystem::file_size(out_path);
        auto const file_size_mb = static_cast<double>(file_size) / 1000.0 / 1000.0;

        std::cout << "Achieved: " << (file_size_mb / duration_s) << " MB/s serializing" << std::endl;
    }
}
