#define CATCH_CONFIG_MAIN
#include "../dataframe/dataframe.h"
#include "../dataframe/dataframeproxy.h"
//#include "../dataframe/ColumnIterator.h"
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include "../dataframe/ConstColumnIterator.h"
#include "../dataframe/grouper.h"
#include "catch.hpp"

using std::string;
using std::vector;

TEST_CASE("DataFrame head", "[head]") {
    vector<string> key1({"a", "a", "b", "b", "a"});
    vector<string> key2({"one", "two", "one", "two", "one"});
    vector<double> data1({1, 2, 3, 4, 5});
    vector<double> data2({11, 12, 13, 14, 15});
    DataFrame df({"key1", "key2", "data1", "data2"}, key1, key2, data1, data2);
    head t;
    Statistic *p = &t;
    DataFrame::Grouper<string> grouper =
        df.groupby<string>(df.cbegin<string>("key1"));
    DataFrame df_res1 = grouper.summarize(p, {"data1"});
    DataFrame df_res2 = grouper.summarize(p);
    DataFrame df_res3 = df.summarize(p);
    REQUIRE(df_res1.size().second == 1);
    REQUIRE(df_res1.loc({"b"}, "data1") == 3.);
    REQUIRE(df_res2.size().second == (df.size().second - 1));
    REQUIRE(df_res2.loc({"a"}, "data2") == 11.);
    REQUIRE(df_res3.size().second == df.size().second);
    REQUIRE(df_res3.size().first == 1);
    REQUIRE(std::string(df_res3.loc({"head"}, "key2")) == "one");
}

TEST_CASE("DataFrame mean", "[mean]") {
    vector<string> key1({"a", "a", "b", "b", "a"});
    vector<string> key2({"one", "two", "one", "two", "one"});
    vector<double> data1({1, 1, 3, 4, 5});
    vector<double> data2({11, 12, 13, 14, 15});
    DataFrame df({"key1", "key2", "data1", "data2"}, key1, key2, data1, data2);
    mean t;
    Statistic *p = &t;
    DataFrame::Grouper<string, double> grouper = df.groupby<string, double>(
        df.cbegin<string>("key1"), df.cbegin<double>("data1"));
    REQUIRE_THROWS_AS(grouper.summarize(p, {"key2"}), std::invalid_argument);
    DataFrame df_res1 = grouper.summarize(p, {"data2"});
    DataFrame df_res2 = df.summarize(p);
    REQUIRE(df_res1.size().first == 4);
    REQUIRE(df_res1.loc({"a", 1.}, "data2") == 11.5);
    REQUIRE(df_res2.loc({"mean"}, "data2") == 13.);
    REQUIRE(df_res2.size().first == 1);
}

TEST_CASE("DataFrame different_col", "[different_col]") {
    vector<string> key1({"a", "a", "b", "b", "a"});
    vector<string> key1_fake({"a", "b", "b", "a"});
    vector<string> key2({"one", "two", "one", "two", "one"});
    vector<double> data1({1, 1, 3, 4, 5});
    vector<double> data2({11, 12, 13, 14, 15});
    DataFrame df({"key1", "data1", "data2"}, key1, data1, data2);
    DataFrame df2({"col"}, key1);
    DataFrame df3({"key1"}, key1_fake);
    REQUIRE_THROWS_AS(df.groupby(df2.cbegin<string>("key1")),
                      std::out_of_range);
    REQUIRE_THROWS_AS(df.groupby(df3.cbegin<string>("key1")),
                      std::invalid_argument);
}
