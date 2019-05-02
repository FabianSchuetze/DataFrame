#include "dataframe/dataframe.h"
#include <vector>
#include <string>
#include <iostream>
using std::vector;
using std::string;

int main() {
    vector<string> idx = {"January 2019", "February 2019"};
    vector<string> names = {"Stock Price", "Dividends"};
    vector<vector<double>> data;
    double sp1 = 12., sp2 = 10.;
    double d1 = 0.5, d2 = 0.6;
    data = {{sp1, sp2}, {d1, d2}};

    DataFrame df1;
    //DataFrame df2(idx);
    DataFrame df3(idx, names, data);
    df3.print();

    //DataFrame df4 = df3 + df3;
    //DataFrame df5 = df3({"January 2019", "February 2019"});
    DataFrame df5 = df3({"January 2019", "February 2019"});
    //std::cout <<*test[0].at(1)<< std::endl;
    df5.print();
    
    //DataFrame df6 = df3("January 2019", "Dividends");
    //df6.print();
    return 0;
}
