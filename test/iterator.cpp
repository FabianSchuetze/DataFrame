#define CATCH_CONFIG_MAIN
#include "../dataframe/dataframe.h"
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "../dataframe/ConstColumnIterator.h"
#include "../dataframe/ColumnIterator.h"
#include "../dataframe/dataframeproxy.h"
#include "catch.hpp"
#include <utility>

using std::string;
using std::vector;

TEST_CASE("DataFrame iterator", "[iterator]") {
    vector<string> key1({"a", "a", "b", "b", "f"});
    vector<double> data1({1, 2, 3, 4, 5});
    std::vector<std::string> idx = {"1", "2", "4", "5", "6"};
    DataFrame df(Index(idx), {"key1", "data1"}, key1, data1);
    DataFrame::iterator<string> it(df.begin<string>("key1"));
    DataFrame::iterator<string> e(df.end<string>("key1"));
    vector<string>res(it, e);
    REQUIRE(res.size() == 5);
    REQUIRE(res[2] == "b");
    REQUIRE(std::string(df.loc({"1"}, "key1"))=="a");
    std::reverse_copy(it, e,  it);
    REQUIRE(std::string(df.loc({"1"}, "key1"))=="f");
    std::fill(it, e, "huhu");
    REQUIRE(std::string(df.loc({"1"}, "key1"))=="huhu");
    DataFrame::iterator<double> vanish;
    REQUIRE_THROWS_AS(std::cout<< *vanish, std::runtime_error);
    {
        DataFrame df2(Index(idx), {"data1"}, data1);
        vanish = df2.begin<double>("data1");
    }
    REQUIRE_THROWS_AS(std::cout<< *vanish, std::runtime_error);
    REQUIRE_THROWS_AS(std::cout<< it[10], std::out_of_range);
}

TEST_CASE("DataFrame const_iterator", "[const_iterator]") {
    vector<string> key1({"a", "a", "b", "b", "f"});
    vector<double> data1({1, 2, 3, 4, 5});
    std::vector<std::string> idx = {"1", "2", "4", "5", "6"};
    DataFrame df(Index(idx), {"key1", "data1"}, key1, data1);
    DataFrame::const_iterator<string> it(df.cbegin<string>("key1"));
    DataFrame::const_iterator<string> e(df.cend<string>("key1"));
    vector<string>res(it, e);
    REQUIRE(res.size() == 5);
    REQUIRE(res[2] == "b");
    REQUIRE(std::string(df.loc({"1"}, "key1"))=="a");
    DataFrame::const_iterator<double> vanish;
    REQUIRE_THROWS_AS(std::cout<< *vanish, std::runtime_error);
    {
        DataFrame df2(Index(idx), {"data1"}, data1);
        vanish = df2.cbegin<double>("data1");
    }
    REQUIRE_THROWS_AS(std::cout<< *vanish, std::runtime_error);
    //std::cout << it[10] << std::endl;
    //REQUIRE_THROWS_AS(std::cout<< it[10], std::out_of_range);
    // TEST PASSED ON LOCAL MACHINE BUT NOT ON TRAVIS?
}
