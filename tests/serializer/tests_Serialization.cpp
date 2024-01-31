#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

template<writer::OutputFormat F>
bool serialize(const Quad& q, void *buffer, writer::Cursor &cursor, writer::FlushFunc flush, writer::SerializationState* state) {
    if constexpr (F == writer::OutputFormat::NTriples)
        return q.serialize_ntriples(buffer, &cursor, flush);
    else if constexpr (F == writer::OutputFormat::NQuads)
        return q.serialize_nquad(buffer, &cursor, flush);
    else if constexpr (F == writer::OutputFormat::Turtle)
        return q.serialize_turtle(*state, buffer, &cursor, flush);
    else if constexpr (F == writer::OutputFormat::TriG)
        return q.serialize_trig(*state, buffer, &cursor, flush);
}

template<writer::OutputFormat F>
std::string write_basic_data(){
    writer::StringWriter ser{};
    writer::SerializationState st{};
    if constexpr (writer::format_has_prefix<F>)
        if (!writer::SerializationState::begin(&ser.buffer(), &ser.cursor(), &writer::StringWriter::flush))
            FAIL("state failed");
    Quad q{IRI::make("http://ex/graph"), IRI::make("http://ex/sub"), IRI::make("http://www.w3.org/1999/02/22-rdf-syntax-ns#type"), IRI::make("http://ex/obj")};
    if (!serialize<F>(q, &ser.buffer(), ser.cursor(), &writer::StringWriter::flush, &st))
        FAIL("write failed");
    q.graph() = Node::make_null();
    q.object() = Literal::make_typed_from_value<datatypes::xsd::Int>(5);
    if (!serialize<F>(q, &ser.buffer(), ser.cursor(), &writer::StringWriter::flush, &st))
        FAIL("write failed");
    if constexpr (writer::format_has_prefix<F>)
        if (!st.flush(&ser.buffer(), &ser.cursor(), &writer::StringWriter::flush))
            FAIL("flush failed");
    return ser.finalize();
}
template<bool HasGraph, parser::ParsingFlag F>
void check_basic_data(const std::string &i) {
    using namespace parser;
    std::istringstream iss{i};
    IStreamQuadIterator qit{iss, F | ParsingFlag::KeepBlankNodeIds};
    CHECK(qit != std::default_sentinel);
    CHECK(qit->value().subject() == IRI::make("http://ex/sub"));
    CHECK(qit->value().predicate() == IRI::make("http://www.w3.org/1999/02/22-rdf-syntax-ns#type"));
    CHECK(qit->value().object() == IRI::make("http://ex/obj"));
    if constexpr (HasGraph)
        CHECK(qit->value().graph() == IRI::make("http://ex/graph"));
    else
        CHECK(qit->value().graph() == IRI::make(""));
    ++qit;
    CHECK(qit != std::default_sentinel);
    CHECK(qit->value().subject() == IRI::make("http://ex/sub"));
    CHECK(qit->value().predicate() == IRI::make("http://www.w3.org/1999/02/22-rdf-syntax-ns#type"));
    CHECK(qit->value().object() == Literal::make_typed_from_value<datatypes::xsd::Int>(5));
    CHECK(qit->value().graph() == IRI::make(""));
    ++qit;
    CHECK(qit == std::default_sentinel);
}

TEST_CASE("basic ntriple") {
    CHECK(write_basic_data<writer::OutputFormat::NTriples>() == "<http://ex/sub> <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <http://ex/obj> .\n<http://ex/sub> <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> \"5\"^^<http://www.w3.org/2001/XMLSchema#int> .\n");
    check_basic_data<false, parser::ParsingFlag::NTriples>(write_basic_data<writer::OutputFormat::NTriples>());
}

TEST_CASE("basic nquad") {
    CHECK(write_basic_data<writer::OutputFormat::NQuads>() == "<http://ex/sub> <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <http://ex/obj> <http://ex/graph> .\n<http://ex/sub> <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> \"5\"^^<http://www.w3.org/2001/XMLSchema#int> .\n");
    check_basic_data<true, parser::ParsingFlag::NQuads>(write_basic_data<writer::OutputFormat::NQuads>());
}

TEST_CASE("basic turtle") {
    CHECK(write_basic_data<writer::OutputFormat::Turtle>() == "@prefix xsd: <http://www.w3.org/2001/XMLSchema#> .\n@prefix rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#> .\n<http://ex/sub> a <http://ex/obj> ,\n\"5\"^^xsd:int .\n");
    check_basic_data<false, parser::ParsingFlag::Turtle>(write_basic_data<writer::OutputFormat::Turtle>());
}

TEST_CASE("basic trig") {
    CHECK(write_basic_data<writer::OutputFormat::TriG>() == "@prefix xsd: <http://www.w3.org/2001/XMLSchema#> .\n@prefix rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#> .\n<http://ex/graph> {\n<http://ex/sub> a <http://ex/obj> .\n}\n<http://ex/sub> a \"5\"^^xsd:int .\n");
    check_basic_data<true, parser::ParsingFlag::TriG>(write_basic_data<writer::OutputFormat::TriG>());
}

template<writer::OutputFormat F>
std::string write_ext_data() {
    writer::StringWriter ser{};
    writer::SerializationState st{};
    if constexpr (writer::format_has_prefix<F>)
        if (!writer::SerializationState::begin(&ser.buffer(), &ser.cursor(), &writer::StringWriter::flush))
            FAIL("state failed");
    Quad q{IRI::make("http://ex/graph"), IRI::make("http://ex/sub"), IRI::make("http://ex/pred"), IRI::make("http://ex/obj")};
    if (!serialize<F>(q, &ser.buffer(), ser.cursor(), &writer::StringWriter::flush, &st))
        FAIL("write failed");

    q.object() = Literal::make_typed_from_value<datatypes::xsd::Integer>(5);
    if (!serialize<F>(q, &ser.buffer(), ser.cursor(), &writer::StringWriter::flush, &st))
        FAIL("write failed");

    q.object() = Literal::make_typed_from_value<datatypes::xsd::Double>(5.0);
    if (!serialize<F>(q, &ser.buffer(), ser.cursor(), &writer::StringWriter::flush, &st))
        FAIL("write failed");

    q.object() = Literal::make_typed<datatypes::xsd::Decimal>("4.2");
    if (!serialize<F>(q, &ser.buffer(), ser.cursor(), &writer::StringWriter::flush, &st))
        FAIL("write failed");

    q.predicate() = IRI::make("http://www.w3.org/1999/02/22-rdf-syntax-ns#type");
    q.object() = Literal::make_typed_from_value<datatypes::xsd::Boolean>(true);
    if (!serialize<F>(q, &ser.buffer(), ser.cursor(), &writer::StringWriter::flush, &st))
        FAIL("write failed");

    if constexpr (writer::format_has_prefix<F>)
        if (!st.flush(&ser.buffer(), &ser.cursor(), &writer::StringWriter::flush))
            FAIL("flush failed");
    return ser.finalize();
}
template<parser::ParsingFlag F>
void check_ext_data(const std::string &i) {
    using namespace parser;
    std::istringstream iss{i};
    IStreamQuadIterator qit{iss, F | ParsingFlag::KeepBlankNodeIds};

    CHECK(qit != std::default_sentinel);
    CHECK(qit->value().subject() == IRI::make("http://ex/sub"));
    CHECK(qit->value().predicate() == IRI::make("http://ex/pred"));
    CHECK(qit->value().object() == IRI::make("http://ex/obj"));
    if constexpr (F == parser::ParsingFlag::TriG)
        CHECK(qit->value().graph() == IRI::make("http://ex/graph"));
    else
        CHECK(qit->value().graph() == IRI::make(""));

    ++qit;
    CHECK(qit != std::default_sentinel);
    CHECK(qit->value().subject() == IRI::make("http://ex/sub"));
    CHECK(qit->value().predicate() == IRI::make("http://ex/pred"));
    CHECK(qit->value().object() == Literal::make_typed_from_value<datatypes::xsd::Integer>(5));
    if constexpr (F == parser::ParsingFlag::TriG)
        CHECK(qit->value().graph() == IRI::make("http://ex/graph"));
    else
        CHECK(qit->value().graph() == IRI::make(""));

    ++qit;
    CHECK(qit != std::default_sentinel);
    CHECK(qit->value().subject() == IRI::make("http://ex/sub"));
    CHECK(qit->value().predicate() == IRI::make("http://ex/pred"));
    CHECK(qit->value().object() == Literal::make_typed_from_value<datatypes::xsd::Double>(5.0));
    if constexpr (F == parser::ParsingFlag::TriG)
        CHECK(qit->value().graph() == IRI::make("http://ex/graph"));
    else
        CHECK(qit->value().graph() == IRI::make(""));

    ++qit;
    CHECK(qit != std::default_sentinel);
    CHECK(qit->value().subject() == IRI::make("http://ex/sub"));
    CHECK(qit->value().predicate() == IRI::make("http://ex/pred"));
    CHECK(qit->value().object() == Literal::make_typed<datatypes::xsd::Decimal>("4.2"));
    if constexpr (F == parser::ParsingFlag::TriG)
        CHECK(qit->value().graph() == IRI::make("http://ex/graph"));
    else
        CHECK(qit->value().graph() == IRI::make(""));

    ++qit;
    CHECK(qit != std::default_sentinel);
    CHECK(qit->value().subject() == IRI::make("http://ex/sub"));
    CHECK(qit->value().predicate() == IRI::make("http://www.w3.org/1999/02/22-rdf-syntax-ns#type"));
    CHECK(qit->value().object() == Literal::make_typed_from_value<datatypes::xsd::Boolean>(true));
    if constexpr (F == parser::ParsingFlag::TriG)
        CHECK(qit->value().graph() == IRI::make("http://ex/graph"));
    else
        CHECK(qit->value().graph() == IRI::make(""));

    ++qit;
    CHECK(qit == std::default_sentinel);
}
template<writer::OutputFormat F, parser::ParsingFlag I>
void extended_tests() {
    std::string data = write_ext_data<F>();
    if constexpr (F == writer::OutputFormat::Turtle)
        CHECK(data == "@prefix xsd: <http://www.w3.org/2001/XMLSchema#> .\n@prefix rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#> .\n<http://ex/sub> <http://ex/pred> <http://ex/obj> ,\n5 ,\n5.0E0 ,\n4.2 ;\na true .\n");
    else
        CHECK(data == "@prefix xsd: <http://www.w3.org/2001/XMLSchema#> .\n@prefix rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#> .\n<http://ex/graph> {\n<http://ex/sub> <http://ex/pred> <http://ex/obj> ,\n5 ,\n5.0E0 ,\n4.2 ;\na true .\n}\n");
    check_ext_data<I>(data);
}
TEST_CASE("extended") {
    // ntriples & nquads don't support any of the syntax tested here
    extended_tests<writer::OutputFormat::Turtle, parser::ParsingFlag::Turtle>();
    extended_tests<writer::OutputFormat::TriG, parser::ParsingFlag::TriG>();
}

static_assert(datatypes::registry::util::ConstexprString("abc")+datatypes::registry::util::ConstexprString("def") == datatypes::registry::util::ConstexprString("abcdef"));
static_assert((datatypes::registry::util::ConstexprString("abc")+datatypes::registry::util::ConstexprString("def")).size() == 7);
