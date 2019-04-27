#include "dataframe.h"
#include <vector>
#include <string>
#include <iostream>
using std::vector;
using std::string;

int main() {
    vector<string> idx;
    idx.push_back("January 2019");
    idx.push_back("February 2019");
    std::cout << "here" << std::endl;
    vector<vector<double>> data;
    vector<double> stock_price;
    stock_price.push_back(10.);
    stock_price.push_back(12.);
    data.push_back(stock_price);

    vector<double> dividends;
    dividends.push_back(0.4);
    dividends.push_back(0.8);
    data.push_back(dividends);
    std::cout << "here" << std::endl;
    
    vector<string> names;
    names.push_back("Stock Price");
    names.push_back("Dividends");

    DataFrame df1;
    DataFrame df2(idx);
    DataFrame df3(idx, names, data);
    
    vector<double> res = df3("January 2019");
    std::cout << res[1] << std::endl;

    double t = df3("January 2019", "Stock Price");
    std::cout << t << std::endl;
    return 0;
}
