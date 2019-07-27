#define CATCH_CONFIG_MAIN
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include "catch.hpp"
#include "../dataframe/dataframe.h"
#include "../dataframe/dataframeproxy.h"
#include "../dataframe/ColumnIterator.h"

using std::string;
using std::vector;

TEST_CASE( "DataFrame drop", "[drop]" ) {
    std::ifstream infile("droprows.csv");
    DataFrame df1(infile);
    DataFrame df2 = df1;
    REQUIRE( df1.size().first == df2.size().first);
    REQUIRE( df1.size().second == df2.size().second);
    REQUIRE( df1.use_count("date") == 2);
    df1.dropna();
    REQUIRE( df1.size().first == 1);
    REQUIRE( df2.size().first == 4);
    REQUIRE( df1.use_count("prob") == 2); //the underlying data is not changed
    df1.drop_row({"f"});
    REQUIRE( df1.size().first == 1);
    std::deque<Index::ele> del;
    del.push_back(5.);
    df1.drop_row(del);
    REQUIRE( df1.size().first == 0);
    std::vector<std::deque<Index::ele>> rows({{-10.}, {5.}});
    REQUIRE( df2.is_contigious());
    df2.drop_row(rows);
    df2.make_contigious();
    REQUIRE( df2.is_contigious());
    REQUIRE( df2.size().first == 2);
    df1.drop_column("date");
    REQUIRE( df1.size().second == 3);
    REQUIRE_THROWS_AS(df1.drop_column("date"), std::out_of_range);
    REQUIRE( df2.is_contigious());
    df2.sort_by_index();
    REQUIRE(!df2.is_contigious());
}

TEST_CASE( "DataFrame replace", "[replace]" ) {
    std::ifstream infile("droprows.csv");
    DataFrame df1(infile);
    DataFrame df2 = df1;
    REQUIRE( df2.use_count("is_payout") == 2);
    df2.fill_na("is_payout", 1.);
    REQUIRE( df2.use_count("is_payout") == 1);
    REQUIRE( df2.use_count("actual") == 2);
    REQUIRE_THROWS_AS(df2.fill_na("huhu", 1.), std::out_of_range);
}

TEST_CASE( "DataFrame names", "[names]" ) {
    std::ifstream infile("droprows.csv");
    DataFrame df1(infile);
    vector<string>res = df1.get_column_names();
    vector<string>res_double = df1.get_column_names<double>();
    REQUIRE(res.size() == 4);
    REQUIRE(res_double.size() == 3);
}
