#define CATCH_CONFIG_MAIN
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include "catch.hpp"
//#include "../dataframe/ColumnIterator.h"
//#include "../dataframe/ConstColumnIterator.h"
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
    //std::cout << df1 << std::endl;
    //std::cout << df2 << std::endl;
    //std::cout << df3 << std::endl;
    REQUIRE( df3.size().first == df1.size().first);
}

