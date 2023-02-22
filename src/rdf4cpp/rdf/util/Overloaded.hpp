#ifndef RDF4CPP_RDF_UTIL_OVERLOADED_HPP
#define RDF4CPP_RDF_UTIL_OVERLOADED_HPP

namespace rdf4cpp::rdf::util {

template<typename ...Fs>
struct Overloaded : Fs... {
    using Fs::operator()...;
};

template<typename ...Fs>
Overloaded(Fs...) -> Overloaded<Fs...>;

}  //namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_RDF_UTIL_OVERLOADED_HPP
