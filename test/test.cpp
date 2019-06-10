#define CATCH_CONFIG_MAIN
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include "catch.hpp"
#include "../dataframe/ColumnIterator.h"
#include "../dataframe/ConstColumnIterator.h"
#include "../dataframe/dataframe.h"
#include "../dataframe/dataframeproxy.h"

TEST_CASE( "DataFrame size", "[size]" ) {
    DataFrame df1;
    REQUIRE( df1.size().first == 0 );
}
