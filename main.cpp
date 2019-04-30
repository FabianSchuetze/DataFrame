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
    vector<double> stock_price = {12., 10.};
    vector<double> dividends = {0.4, 0.8};
    data.push_back(stock_price);
    data.push_back(dividends);
    std::cout << data.size() << std::endl;

    //DataFrame df1;
    //DataFrame df4;
    //DataFrame df2(idx);
    DataFrame df3(idx, names, data);
    DataFrame newdf;
    newdf = df3;
    df3.print();
    
    //newdf = df3("January 2019");
    newdf.print();
    //std::cout << res[0] << std::endl;

    //double t = df3("January 2019", "Dividends");
    //std::cout << t << std::endl;
    //df3.print();
    //return 0;
}
    //vector<p> cont;
    ////p p2 = &stock_price; 
    //typedef vector<vector<double> >::const_iterator iter;
    //iter abc = data.begin();
    //typedef vector<double>::const_iterator iter2;
    //iter2 res = abc->begin();
    //std::cout << *res << std::endl;
    ////cont.push_back(res);
    //cont.push_back(&data[0]);
    //cont.push_back(&data[1]);
    //vector<double> abv = *cont[0];
    //std::cout << abv[0] << std::endl;
