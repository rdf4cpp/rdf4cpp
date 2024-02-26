#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("Literal short type") {
    std::string buf;
    writer::StringWriter ser{buf};
    Literal::make_typed<datatypes::xsd::Date>("2042-5-4").serialize_short_form(ser);
    writer::write_str("\n", ser);
    Literal::make_typed<datatypes::xsd::Boolean>("true").serialize_short_form(ser);
    writer::write_str("\n", ser);
    Literal::make_typed<datatypes::xsd::Float>("4").serialize_short_form(ser);
    writer::write_str("\n", ser);
    Literal::make_typed<datatypes::xsd::UnsignedByte>("4").serialize_short_form(ser);
    writer::write_str("\n", ser);
    Literal::make_typed<datatypes::xsd::Integer>("4").serialize_short_form(ser);
    ser.finalize();

    CHECK_EQ(buf, "\"2042-05-04\"^^xsd:date\ntrue\n\"4.0E0\"^^xsd:float\n\"4\"^^xsd:unsignedByte\n4");
}

template<writer::OutputFormat F>
bool serialize(const Quad& q, void *buffer, writer::Cursor &cursor, writer::FlushFunc flush, writer::SerializationState* state) {
    if constexpr (F == writer::OutputFormat::NTriples)
        return q.serialize_ntriples(buffer, &cursor, flush);
    else if constexpr (F == writer::OutputFormat::NQuads)
        return q.serialize_nquads(buffer, &cursor, flush);
    else if constexpr (F == writer::OutputFormat::Turtle)
        return q.serialize_turtle(*state, buffer, &cursor, flush);
    else if constexpr (F == writer::OutputFormat::TriG)
        return q.serialize_trig(*state, buffer, &cursor, flush);
}

template<writer::OutputFormat F>
std::string write_basic_data(){
    std::string buf;
    writer::StringWriter ser{buf};
    writer::SerializationState st{};
    if constexpr (writer::format_has_prefix<F>)
        if (!writer::SerializationState::begin(&ser.buffer(), &ser.cursor(), &writer::StringWriter::flush))
            FAIL("state failed");
    Quad q{IRI::make("http://ex/graph"), IRI::make("http://ex/sub"), IRI::make("http://www.w3.org/1999/02/22-rdf-syntax-ns#type"), IRI::make("http://ex/obj")};
    if (!serialize<F>(q, &ser.buffer(), ser.cursor(), &writer::StringWriter::flush, &st))
        FAIL("write failed");
    q.graph() = IRI::make("http://ex/graph2");
    q.object() = Literal::make_typed_from_value<datatypes::xsd::Int>(5);
    if (!serialize<F>(q, &ser.buffer(), ser.cursor(), &writer::StringWriter::flush, &st))
        FAIL("write failed");
    if constexpr (writer::format_has_prefix<F>)
        if (!st.flush(&ser.buffer(), &ser.cursor(), &writer::StringWriter::flush))
            FAIL("flush failed");
    ser.finalize();
    return buf;
}
Graph get_graph() {
    Graph gd{};
    Statement q{IRI::make("http://ex/sub"), IRI::make("http://www.w3.org/1999/02/22-rdf-syntax-ns#type"), IRI::make("http://ex/obj")};
    gd.add(q);
    q.object() = Literal::make_typed_from_value<datatypes::xsd::Int>(5);
    gd.add(q);
    return gd;
}
Dataset get_dataset() {
    Dataset gd{};
    Quad q{IRI::make("http://ex/graph"), IRI::make("http://ex/sub"), IRI::make("http://www.w3.org/1999/02/22-rdf-syntax-ns#type"), IRI::make("http://ex/obj")};
    gd.add(q);
    q.graph() = IRI::make("http://ex/graph2");
    q.object() = Literal::make_typed_from_value<datatypes::xsd::Int>(5);
    gd.add(q);
    return gd;
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
    if constexpr (HasGraph)
        CHECK(qit->value().graph() == IRI::make("http://ex/graph2"));
    else
        CHECK(qit->value().graph() == IRI::make(""));
    ++qit;
    CHECK(qit == std::default_sentinel);
}

TEST_CASE("basic ntriple") {
    const std::string  d = write_basic_data<writer::OutputFormat::NTriples>();
    CHECK(d == "<http://ex/sub> <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <http://ex/obj> .\n<http://ex/sub> <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> \"5\"^^<http://www.w3.org/2001/XMLSchema#int> .\n");

    auto const res = writer::StringWriter::oneshot([](auto &w) noexcept {
        return get_graph().serialize(w);
    });

    CHECK_EQ(res, d);
    check_basic_data<false, parser::ParsingFlag::NTriples>(d);
}

TEST_CASE("basic nquad") {
    const std::string d = write_basic_data<writer::OutputFormat::NQuads>();
    CHECK(d == "<http://ex/sub> <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <http://ex/obj> <http://ex/graph> .\n<http://ex/sub> <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> \"5\"^^<http://www.w3.org/2001/XMLSchema#int> <http://ex/graph2> .\n");

    auto const res = writer::StringWriter::oneshot([](auto &w) noexcept {
        return get_dataset().serialize(w);
    });

    CHECK_EQ(res, d);
    check_basic_data<true, parser::ParsingFlag::NQuads>(d);
}

TEST_CASE("basic turtle") {
    const std::string d = write_basic_data<writer::OutputFormat::Turtle>();
    CHECK(d == "@prefix xsd: <http://www.w3.org/2001/XMLSchema#> .\n@prefix rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#> .\n<http://ex/sub> a <http://ex/obj> ,\n\"5\"^^xsd:int .\n");

    auto const res = writer::StringWriter::oneshot([](auto &w) noexcept {
        return get_graph().serialize_turtle(w);
    });

    CHECK_EQ(res, d);
    check_basic_data<false, parser::ParsingFlag::Turtle>(d);
}

TEST_CASE("basic trig") {
    const std::string d = write_basic_data<writer::OutputFormat::TriG>();
    CHECK(d == "@prefix xsd: <http://www.w3.org/2001/XMLSchema#> .\n@prefix rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#> .\n<http://ex/graph> {\n<http://ex/sub> a <http://ex/obj> .\n}\n<http://ex/graph2> {\n<http://ex/sub> a \"5\"^^xsd:int .\n}\n");

    auto const res = writer::StringWriter::oneshot([](auto &w) noexcept {
        return get_dataset().serialize_trig(w);
    });

    CHECK_EQ(res, d);
    check_basic_data<true, parser::ParsingFlag::TriG>(d);
}

template<writer::OutputFormat F>
std::string write_ext_data() {
    std::string buf;
    writer::StringWriter ser{buf};
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

    ser.finalize();
    return buf;
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
