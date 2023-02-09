#ifndef RDF4CPP_OVERLOADED_HPP
#define RDF4CPP_OVERLOADED_HPP

namespace rdf4cpp::rdf::storage::util {

template<typename ...Fs>
struct Overloaded : Fs... {
    using Fs::operator()...;
};

template<typename ...Fs>
Overloaded(Fs...) -> Overloaded<Fs...>;

}  //namespace rdf4cpp::rdf::storage::util

#endif  //RDF4CPP_OVERLOADED_HPP
