#include "dataframe/dataframe.h"
#include "dataframe/dataframeproxy.h"
#include "dataframe/ColumnIterator.h"
#include "dataframe/ConstColumnIterator.h"
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <cmath>
#include <chrono>
#include <numeric>

using namespace std::chrono;

using std::vector;
using std::string;

vector<string> get_columns() {
    vector<string> res;
    vector<string> cols = {"total_sales_volume", "is_sale"};
    vector<string> xs ={"initial", "first", "second", "third", "fourth",
          "fifth", "sixth", "seventh", "eight", "ninth",
          "tenth"};
    for (auto x : xs) {
        string col1 = "saex2_" + x + "_is_payout";
        string col2 = "saex2_" + x + "_actual";
        cols.push_back(col1);
        cols.push_back(col2);
    }
    return cols;
}

void fill_df(DataFrame& df) {
    vector<string> double_names = df.get_column_names<double>();
    for (const string& s : double_names) {
        DataFrame::iter<double> it = df.begin<double>(s);
        DataFrame::iter<double>  e = df.end<double>(s);
        std::transform(it, e, it, [](auto& d) {return std::isnan(d) ? 0 : d;});
    }
}

template <typename T>
void sort_df(DataFrame& df, const std::string& s) {
    df.sort_by_column(s);
}

int main() {
    std::ifstream infile("amits_example.csv"); //make a check about the file
    DataFrame df1 = DataFrame(infile);
    std::cout << df1 << std::endl;
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    df1.dropna();
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>( t2 - t1 ).count();
    std::cout << duration << std::endl;
    std::cout << df1.size().first << std::endl;
    return 0;
}
