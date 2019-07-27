#define CATCH_CONFIG_MAIN
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include "catch.hpp"
#include "../dataframe/dataframe.h"
#include "../dataframe/dataframeproxy.h"

using std::ifstream;

TEST_CASE( "DataFrame csv", "[csv]" ) {
    ifstream infile("csv_example1.csv");
    ifstream wrong_length("csv_wronglength.csv");
    ifstream wrong_type("csv_wrongtype.csv");
    DataFrame df1(infile);
    std::cout << df1 << std::endl;
    REQUIRE(df1.size().first ==1);
    REQUIRE(df1.size().second ==2);
    REQUIRE_THROWS_AS(DataFrame(wrong_length), std::invalid_argument);
    REQUIRE_THROWS_AS(DataFrame(wrong_type), std::invalid_argument);
}
