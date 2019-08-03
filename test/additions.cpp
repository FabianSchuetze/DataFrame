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

TEST_CASE( "DataFrame individual", "[individual]" ) {
    std::vector<double> first = {10, 10, 10, -8};
    std::vector<double> second = {30, -100, 40, 100};
    std::vector<std::string> idx = {"1", "2", "4", "5"};
    std::vector<std::string> col_names = {"first_col"};
    std::vector<std::string> col_names2 = {"second_col"};
    DataFrame df1(Index(idx), col_names, first);
    DataFrame df3 = df1 + 2.;
    REQUIRE( df3.size().second == 1);
    REQUIRE(df3.loc({"1"}, "first_col") == 12.);
    REQUIRE_THROWS_AS(df1 + std::string("fabi"), std::invalid_argument);
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
    std::vector<std::string> first = {"aa", "bb"};
    std::vector<int> idx1 = {1, 1};
    std::vector<int> idx2 = {1, 1};
    std::vector<int> idx3 = {1, 2};
    std::vector<std::string> col_names = {"first_col"};
    DataFrame df1;
    DataFrame df2(Index(idx1, idx3), col_names, first);
    DataFrame df3 = df1 + df2;
    DataFrame df4(Index(idx1, idx1), col_names, first);
    DataFrame df5 = df4 + df2;
    DataFrame df6(Index(idx1), col_names, first);
    REQUIRE( df3.size().first == df1.size().first + df2.size().first);
    REQUIRE( df5.size().first == 3);
    // MUST FIND BETTER COMPARISON FOR STRINGS, IMPLICIT CONVERSION!
    REQUIRE( std::string(df5.loc({1 ,2}, "first_col")) == "NA");
    REQUIRE( std::string(df5.loc({1, 1}, "first_col")) == "aaaa");
    REQUIRE_THROWS_AS( df2 + df6, std::invalid_argument);
}

TEST_CASE( "DataFrame duplicate", "[duplicate]" ) {
    std::vector<std::string> first = {"aa", "bb"};
    std::vector<int> rhs({1, 1});
    std::vector<int> lhs({1,2});
    std::vector<std::string> name({"column_a"});
    DataFrame df1(Index(lhs), name, first);
    DataFrame df2(Index(rhs), name, first);
    DataFrame df3 = df1 + df2;
    std::cout << df3 << std::endl;
    REQUIRE( df3.size().first == 3);
}

TEST_CASE( "DataFrame bool", "[bool]" ) {
    std::vector<double> col = {10, 20};
    std::vector<bool> col_bool = {true, false};
    std::vector<int> idx({1, 2});
    std::vector<std::string> name({"column_a"});
    DataFrame df1(Index(idx), name, col);
    DataFrame df2(Index(idx), name, col_bool);
    DataFrame df3 = df1 + df2;
    REQUIRE( df3.loc({1}, "column_a") == 11);
    REQUIRE( df3.loc({2}, "column_a") == 20);
}
