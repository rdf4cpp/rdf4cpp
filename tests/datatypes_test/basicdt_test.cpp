//
// Created by kaimal on 29.11.21.
//

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf::datatypes;

TEST_CASE("Datatype Integer") {

    auto int_iri = "http://www.w3.org/2001/XMLSchema#int";
    auto input = "2147483647";
    auto output = RegisteredDatatype<xsd::Int>::from_string(input);
    CHECK(output == 2147483647);
    CHECK(RegisteredDatatype<xsd::Int>::datatype_iri() == int_iri);

    input = "-2147483648";
    output = RegisteredDatatype<xsd::Int>::from_string(input);
    CHECK(output == -2147483648);
    CHECK(RegisteredDatatype<xsd::Int>::datatype_iri() == int_iri);
    //std::cout <<"string value "<< input <<" is converted to integer value " << output << std::endl;
}

TEST_CASE("Datatype Short") {

    auto short_iri = "http://www.w3.org/2001/XMLSchema#short";
    auto input = "32767";
    auto output = RegisteredDatatype<xsd::Short>::from_string(input);
    CHECK(output == 32767);
    CHECK(RegisteredDatatype<xsd::Int>::datatype_iri() == short_iri);

    input = "-32768";
    output = RegisteredDatatype<xsd::Short>::from_string(input);
    CHECK(output == -32768);
    CHECK(RegisteredDatatype<xsd::Int>::datatype_iri() == short_iri);

    /*input = "32768";
    output = RegisteredDatatype<xsd::Short>::from_string(input);
    CHECK(output == 32768);

    input = "-32769";
    output = RegisteredDatatype<xsd::Short>::from_string(input);
    CHECK(output == -32769);

    input = "65800";
    output = RegisteredDatatype<xsd::Short>::from_string(input);
    CHECK(output == 65800);*/
    //std::cout <<"string value "<< input <<" is converted to integer value " << output << std::endl;
}

TEST_CASE("Datatype Byte") {

    auto input = "127";
    auto output = RegisteredDatatype<xsd::Byte>::from_string(input);
    CHECK(output == 127);

    input = "-128";
    output = RegisteredDatatype<xsd::Byte>::from_string(input);
    CHECK(output == -128);

    /*input = "128";
    output = RegisteredDatatype<xsd::Byte>::from_string(input);
    CHECK(output == 128);

    input = "-129";
    output = RegisteredDatatype<xsd::Byte>::from_string(input);
    CHECK(output == -129);

    input = "255";
    output = RegisteredDatatype<xsd::Byte>::from_string(input);
    CHECK(output == 255);*/
    //std::cout <<"string value "<< input <<" is converted to integer value " << output << std::endl;
}

TEST_CASE("Datatype UnsignedInt") {

    auto input = "0";
    auto output = RegisteredDatatype<xsd::UnsignedInt>::from_string(input);
    CHECK(output == 0);

    input = "4294967295";
    output = RegisteredDatatype<xsd::UnsignedInt>::from_string(input);
    CHECK(output == 4294967295);
    std::cout <<"string value "<< input <<" is converted to integer value " << output << std::endl;
}

TEST_CASE("Datatype UnsignedShort") {

    auto input = "0";
    auto output = RegisteredDatatype<xsd::UnsignedShort>::from_string(input);
    CHECK(output == 0);

    input = "65535";
    output = RegisteredDatatype<xsd::UnsignedShort>::from_string(input);
    CHECK(output == 65535);

    /*input = "32768";
    output = RegisteredDatatype<xsd::UnsignedShort>::from_string(input);
    CHECK(output == 32768);

    input = "-32769";
    output = RegisteredDatatype<xsd::UnsignedShort>::from_string(input);
    CHECK(output == -32769);

    input = "65536";
    output = RegisteredDatatype<xsd::UnsignedShort>::from_string(input);
    CHECK(output == 65536);*/
    //std::cout <<"string value "<< input <<" is converted to integer value " << output << std::endl;
}

TEST_CASE("Datatype UnsignedByte") {

    auto input = "0";
    auto output = RegisteredDatatype<xsd::UnsignedByte>::from_string(input);
    CHECK(output == 0);

    input = "255";
    output = RegisteredDatatype<xsd::UnsignedByte>::from_string(input);
    CHECK(output == 255);

    /*input = "-127";
    output = RegisteredDatatype<xsd::Byte>::from_string(input);
    CHECK(output == -127);

    input = "128";
    output = RegisteredDatatype<xsd::Byte>::from_string(input);
    CHECK(output == 128);

    input = "256";
    output = RegisteredDatatype<xsd::Byte>::from_string(input);
    CHECK(output == 256);*/
    //std::cout <<"string value "<< input <<" is converted to integer value " << output << std::endl;
}

TEST_CASE("Datatype Float") {

    auto input = "1.0";
    auto output = RegisteredDatatype<xsd::Float>::from_string(input);
    CHECK(output == 0);

    input = "222.00";
    output = RegisteredDatatype<xsd::Float>::from_string(input);
    CHECK(output == 255);

    /*input = "-127";
    output = RegisteredDatatype<xsd::Byte>::from_string(input);
    CHECK(output == -127);

    input = "128";
    output = RegisteredDatatype<xsd::Byte>::from_string(input);
    CHECK(output == 128);

    input = "256";
    output = RegisteredDatatype<xsd::Byte>::from_string(input);
    CHECK(output == 256);*/
    //std::cout <<"string value "<< input <<" is converted to integer value " << output << std::endl;
}

TEST_CASE("Datatype Double") {

    auto input = "1.0";
    auto output = RegisteredDatatype<xsd::Double>::from_string(input);
    CHECK(output == 0);

    input = "222.00";
    output = RegisteredDatatype<xsd::Double>::from_string(input);
    CHECK(output == 255);

    /*input = "-127";
    output = RegisteredDatatype<xsd::Byte>::from_string(input);
    CHECK(output == -127);

    input = "128";
    output = RegisteredDatatype<xsd::Byte>::from_string(input);
    CHECK(output == 128);

    input = "256";
    output = RegisteredDatatype<xsd::Byte>::from_string(input);
    CHECK(output == 256);*/
    //std::cout <<"string value "<< input <<" is converted to integer value " << output << std::endl;
}