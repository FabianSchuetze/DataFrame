#include "dataframe/dataframe.h"
#include <vector>
#include <string>
#include <iostream>
#include <map>
using std::vector;
using std::string;
using std::map;

int main() {
    vector<string> idx = {"J", "F"};
    vector<string> names = {"Stock Price", "Dividends"};
    vector<string> names2 = {"Stock Price"};
    vector<vector<double>> data;
    double sp1 = 12., sp2 = 10.;
    double d1 = 0.5, d2 = 0.6;
    data = {{sp1, sp2}, {d1, d2}};
    vector<vector<double>> data2 = {{sp1 + 10, sp2 + 10}};
    DataFrame df1;
    //df1.print();
    DataFrame df3(idx, names, data);
    DataFrame df4(idx, names2, data2);
    //df4.print(10);
    //std::cout << "here\n";
    //df4({"Stock Price"}) = df3({"Dividends"}); // THis also modified df3!!!
    df3.print();
    df3.sort_index();
    df3.print();
    //df4.print(5);
    return 0;
}
