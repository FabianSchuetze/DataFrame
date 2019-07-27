#define CATCH_CONFIG_MAIN
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include "../dataframe/dataframe.h"
#include "../dataframe/dataframeproxy.h"
#include "catch.hpp"

using std::string;
using std::vector;

TEST_CASE("DataFrame wrong_constructors", "[wrong_constructors]") {
    vector<string> key1({"a", "a", "b", "b", "a"});
    vector<string> key2({"one", "two", "one", "two", "one"});
    vector<double> data1({1, 2, 3, 4, 5});
    vector<double> data2({11, 12, 13, 14, 15});
    std::vector<std::string> idx = {"1", "2", "4", "5"};
    REQUIRE_THROWS_AS(
        DataFrame(Index(idx), {"key1", "key2", "data1", "data2"}, key1, key2,
                     data1, data2),
        std::invalid_argument);
    REQUIRE_THROWS_AS(
        DataFrame(Index(idx), {"key2", "data1", "data2"}, key1, key2,
                     data1, data2),
        std::invalid_argument);
}
