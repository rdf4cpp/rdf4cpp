#ifndef RDF4CPP_PRIVATE_OUTPUTFORMAT_HPP
#define RDF4CPP_PRIVATE_OUTPUTFORMAT_HPP

namespace rdf4cpp::rdf::writer {

enum struct OutputFormat {
    NTriples,
    Turtle,
    NQuads,
    TriG
};

template<OutputFormat F>
concept format_has_graph = (F == OutputFormat::NQuads || F == OutputFormat::TriG);

template<OutputFormat F>
concept format_has_prefix = (F == OutputFormat::Turtle || F == OutputFormat::TriG);

} // namespace rdf4cpp::rdf::writer

#endif // RDF4CPP_PRIVATE_OUTPUTFORMAT_HPP
