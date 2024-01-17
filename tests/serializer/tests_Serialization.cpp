#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

template<writer::OutputFormat F>
std::string write_basic_data(){
    writer::StringWriter ser{};
    writer::SerializationState st{};
    st.prefixes.emplace_back("http://ex/", "e");
    if (!writer::serialize_state<F>(&ser.buffer(), ser.cursor(), &writer::StringWriter::flush, &st))
        FAIL("state failed");
    Quad q{IRI::make("http://ex/graph"), IRI::make("http://ex/sub"), IRI::make("http://ex/pred"), IRI::make("http://ex/obj")};
    if (!writer::serialize<F>(q, &ser.buffer(), ser.cursor(), &writer::StringWriter::flush, &st))
        FAIL("write failed");
    q.graph() = Node::make_null();
    q.object() = Literal::make_typed_from_value<datatypes::xsd::Int>(5);
    if (!writer::serialize<F>(q, &ser.buffer(), ser.cursor(), &writer::StringWriter::flush, &st))
        FAIL("write failed");
    if (!writer::flush_state<F>(&ser.buffer(), ser.cursor(), &writer::StringWriter::flush, &st))
        FAIL("flush failed");
    return ser.finalize();
}

TEST_CASE("basic ntriple") {
    CHECK(write_basic_data<writer::OutputFormat::NTriples>() == "<http://ex/sub> <http://ex/pred> <http://ex/obj> .\n<http://ex/sub> <http://ex/pred> \"5\"^^<http://www.w3.org/2001/XMLSchema#int> .\n");
}

TEST_CASE("basic nquad") {
    CHECK(write_basic_data<writer::OutputFormat::NQuads>() == "<http://ex/sub> <http://ex/pred> <http://ex/obj> <http://ex/graph> .\n<http://ex/sub> <http://ex/pred> \"5\"^^<http://www.w3.org/2001/XMLSchema#int> .\n");
}

TEST_CASE("basic turtle") {
    CHECK(write_basic_data<writer::OutputFormat::Turtle>() == "@prefix xsd: <http://www.w3.org/2001/XMLSchema#> .\n@prefix e: <http://ex/> .\ne:sub e:pred e:obj ,\n\"5\"^^xsd:int .\n");
}

TEST_CASE("basic trig") {
    CHECK(write_basic_data<writer::OutputFormat::TriG>() == "@prefix xsd: <http://www.w3.org/2001/XMLSchema#> .\n@prefix e: <http://ex/> .\ne:graph {\ne:sub e:pred e:obj .\n}\ne:sub e:pred \"5\"^^xsd:int .\n");
}
