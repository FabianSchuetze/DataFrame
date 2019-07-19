#define CATCH_CONFIG_MAIN
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include "catch.hpp"
#include "../dataframe/dataframe.h"
#include "../dataframe/dataframeproxy.h"

TEST_CASE( "DataFrame unique", "[unique]" ) {
    std::vector<double> first = {10, 10, 10, -8};
    std::vector<double> second = {30, -100, 40, 100};
    std::vector<std::string> idx = {"1", "2", "4", "5"};
    std::vector<std::string> col_names = {"first_col"};
    DataFrame df1(Index(idx), col_names, first);
    DataFrame df2(Index(idx), col_names, second);
    DataFrame df3 = df1 + df2;
    REQUIRE( df3.size().first == df1.size().first);
    REQUIRE( df3.loc({"1"}, "first_col") == 40 );
    REQUIRE( df3.loc({"2"}, "first_col") == -90 );
    REQUIRE( df3.loc({"4"}, "first_col") == 50 );
    REQUIRE( df3.loc({"5"}, "first_col") == 92 );
}

TEST_CASE( "DataFrame col_different", "[col_different]" ) {
    std::vector<double> first = {10, 10, 10, -8};
    std::vector<double> second = {30, -100, 40, 100};
    std::vector<std::string> idx = {"1", "2", "4", "5"};
    std::vector<std::string> col_names = {"first_col"};
    std::vector<std::string> col_names2 = {"second_col"};
    DataFrame df1(Index(idx), col_names, first);
    DataFrame df2(Index(idx), col_names2, second);
    DataFrame df3 = df1 + df2;
    REQUIRE( df3.size().second == 2);
    REQUIRE( std::isnan(df3.loc({"1"}, "first_col")));
    REQUIRE( std::isnan(df3.loc({"5"}, "second_col")));
}

TEST_CASE( "DataFrame index_different", "[index_different]" ) {
    std::vector<double> first = {10, 10, 10, -8};
    std::vector<std::string> idx = {"1", "2", "4", "5"};
    std::vector<std::string> idx2 = {"10", "12", "14", "15"};
    std::vector<std::string> col_names = {"first_col"};
    DataFrame df1(Index(idx), col_names, first);
    DataFrame df2(Index(idx2), col_names, first);
    DataFrame df3 = df1 + df2;
    REQUIRE( df3.size().first == df1.size().first + df2.size().first);
    REQUIRE( df3.size().second == df1.size().second);
    REQUIRE( std::isnan(df3.loc({"1"}, "first_col")));
    REQUIRE( std::isnan(df3.loc({"15"}, "first_col")));
}

TEST_CASE( "DataFrame overlapping", "[overlapping]" ) {
    std::vector<double> first = {10, 10, 10, -8};
    std::vector<std::string> idx1 = {"1", "1"};
    std::vector<std::string> idx2 = {"1", "1"};
    std::vector<std::string> idx3 = {"1", "2"};
    std::vector<std::string> col_names = {"first_col"};
    //DataFrame df1(Index(idx1, idx2), col_names, first);
    DataFrame df1;
    DataFrame df2(Index(idx1, idx3), col_names, first);
    DataFrame df3 = df1 + df2;
    DataFrame df4(Index(idx1, idx1), col_names, first);
    DataFrame df5 = df4 + df2;
    DataFrame df6(Index(idx1), col_names, first);
    std::cout << df3 << std::endl;
    std::cout << df5 << std::endl;
    REQUIRE( df3.size().first == df1.size().first + df2.size().first);
    REQUIRE( df5.size().first == 3);
    REQUIRE( std::isnan(df5.loc({"1", "2"}, "first_col")));
    REQUIRE_THROWS_AS( df5.loc({"1", "1"}, "first_col") == 20., 
            std::runtime_error);
    REQUIRE_THROWS_AS( df2 + df6, std::invalid_argument);
    //REQUIRE( std::isnan(df3.loc({"1"}, "first_col")));
    //REQUIRE( std::isnan(df3.loc({"15"}, "first_col")));
}

