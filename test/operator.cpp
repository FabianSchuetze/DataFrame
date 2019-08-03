#define CATCH_CONFIG_MAIN
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include "catch.hpp"
#include "../dataframe/dataframe.h"
#include "../dataframe/dataframeproxy.h"

using std::vector;
using std::string;
TEST_CASE( "DataFrame smaller", "[smaller]" ) {
    vector<double> first1({30, 40});
    vector<double> first2({10, 20});
    vector<string> names = {"first_col", "second_col"};
    vector<string> rhs_names = {"first_col", "third_col"};
    DataFrame df1 = DataFrame(names, first1, first2);
    DataFrame df2 = df1;
    REQUIRE(std::cout << DataFrame(df1 < 11.) << std::endl);
    df1["string_col"] = rhs_names; 
    REQUIRE_THROWS_AS(DataFrame(df1 < "fabi"), std::invalid_argument);
    REQUIRE_THROWS_AS(DataFrame(df1 < 11.), std::invalid_argument);
    REQUIRE(std::cout << DataFrame(df2 > 11.) << std::endl);
    REQUIRE_THROWS_AS(DataFrame(df1 > "fabi"), std::invalid_argument);
    REQUIRE_THROWS_AS(DataFrame(df1 < 11.), std::invalid_argument);
    vector<string>subset({"first_col", "second_col"});
    REQUIRE(std::cout << DataFrame(df1[subset] < 11.) << std::endl);
}
TEST_CASE( "DataFrame ProxyInsert", "[ProxyInsert]" ) {
    vector<double> first1({30, 40});
    vector<double> second1({-100, -200});
    vector<vector<double>> third1({{-100, -200}, {400, 600}});
    vector<string> first2({"a", "b"});
    vector<string> names = {"first_col"};
    DataFrame df1 = DataFrame(names, first1);
    DataFrame df2 = DataFrame(names, second1);
    REQUIRE(df1.loc({0}, "first_col") == 30.);
    df1["first_col"] = first2;
    REQUIRE(std::string(df1.loc({0}, "first_col")) == std::string("a"));
    df1["first_col"] = df2["first_col"];
    REQUIRE(df1.loc({1}, "first_col") == -200.);
    df1["second_col"] = df2;
    REQUIRE(df1.loc({1}, "second_col") == -200.);
    std::cout << df1 << std::endl;
    df1[vector<string>({"third_col", "fourth_col"})] = third1;
    REQUIRE(df1.loc({1}, "fourth_col") == 600.);
    vector<double> failure({ 40});
    REQUIRE_THROWS_AS(df1["first_col"] = failure, std::invalid_argument);
    std::vector<std::deque<Index::ele>> subset;
    subset = {{0}, {1}};
    REQUIRE_NOTHROW(df1.loc(subset, names) = second1);
    //std::cout << df1 << std::endl;
    //REQUIRE(df1.loc({1}, "first_col") == -100.);
}
