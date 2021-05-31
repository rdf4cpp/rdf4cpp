//
// Created by Lixi Alié Conrads on 5/29/21.
//

#ifndef RDF4CPP_RESULT_H
#define RDF4CPP_RESULT_H

#include <variant>
#include <optional>
#include <iostream>

namespace rdf4cpp::utils::sec {


    ///
    /// The Rust like Result Object to assure that Errors are handled and a null object is taken care of.
    /// <br/><br/>
    /// It is fundamentally the same as a variant.
    ///
    ///
    /// \tparam V  -The Ok class type
    /// \tparam K  -The Error class Type
    ///
    template <class V, class K>
    class Result {

    public:

        ///
        /// Tries to return the Ok Value otherwise will panic exit.
        ///
        /// \return The unwrapped Ok Value.
        ///
        V unwrap() const {
            if (this->is_ok()){
                return std::get<V>(this->value);
            }
            std::cerr << "[Panic!] exit due to sec reasons: Tried to unwrap value, however error value is set." << std::endl ;
            std::exit(1);
        }

        ///
        /// Tries to return the Ok Value otherwise will panic exit with the given message
        ///
        /// \param msg Message that should be written to std err if this contains an error value
        /// \return The unwrapped Ok Value.
        ///
        V expect(std::string msg) const {
            if (this->is_ok()){
                return std::get<V>(this->value);
            }
            std::cerr << "[Panic!]" << msg << std::endl ;
            std::exit(1);
        }

        /// If the Result contains the Ok (V) Object
        ///
        /// \return true if result is valid and ok, false otherwise.
        [[nodiscard]] bool is_ok() const{
            std::holds_alternative<V>(this->value);
        }

        /// If the Result is an error thus contains the Err (K) Object
        ///
        /// \return true if the result is an error, false otherwise
        [[nodiscard]] bool is_err() const{
            std::holds_alternative<K>(this->value);
        }

        /// Converts the result object to an std::optional containing the Ok Value <V> if the Ok value is set,
        /// or a std::nullopt item if the Error value is set.
        ///
        /// \return an std::optional object containing the Ok value if the Ok value is set, otherwise a std::nullopt
        std::optional<V> ok() const{
            if (this->is_ok()){
                return std::optional<V>(std::get<V>(this->value));
            }
            else{
                return std::nullopt;
            }
        }

        /// Converts the result object to an std::optional containing the Error Value <K> if the Error value is set,
        /// or a std::nullopt item if the Ok value is set.
        ///
        /// \return an std::optional object containing the Error value if the Ok value is set, otherwise a std::nullopt
        std::optional<K> err() const{
            if (this->is_err()){
                return std::optional<K>(std::get<K>(this->value));
            }
            return std::nullopt;
        }

    protected:
        std::variant<V, K> value;
        Result() {}
    };



}


#endif //RDF4CPP_RESULT_H
