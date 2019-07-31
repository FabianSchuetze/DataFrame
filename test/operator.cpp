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
