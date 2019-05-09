#include "dataframe/dataframe.h"
#include <vector>
#include <string>
#include <iostream>
#include <map>
using std::vector;
using std::string;
using std::map;

int main() {
    vector<string> idx = {"January 2019", "February 2019"};
    vector<string> names = {"Stock Price", "Dividends"};
    vector<string> names2 = {"Stock Price"};
    vector<vector<double>> data;
    double sp1 = 12., sp2 = 10.;
    double d1 = 0.5, d2 = 0.6;
    data = {{sp1, sp2}, {d1, d2}};
    vector<vector<double>> data2 = {{sp1 + 10, sp2 + 10}};
    DataFrame df1;
    const DataFrame df3(idx, names, data);
    DataFrame df4(idx, names2, data2);
    df3.print();
    df4({"Stock Price"}) = df3({"Dividends"}); // THis also modified df3!!!
    return 0;
}
