#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include "../dataframe/ColumnIterator.h"
#include "../dataframe/ConstColumnIterator.h"
#include "../dataframe/dataframe.h"
#include "../dataframe/dataframeproxy.h"

using namespace std::chrono;

using std::string;
using std::vector;

vector<string> get_columns() {
    vector<string> res;
    vector<string> cols = {"total_sales_volume", "is_sale"};
    vector<string> xs = {"initial", "first", "second", "third",
                         "fourth",  "fifth", "sixth",  "seventh",
                         "eight",   "ninth", "tenth"};
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
        DataFrame::iter<double> e = df.end<double>(s);
        std::transform(it, e, it,
                       [](auto& d) { return std::isnan(d) ? 0 : d; });
    }
}

//template <typename T>
//void sort_df(DataFrame& df, const std::string& s) {
    //df.sort_by_column(s);
//}

int main() {
    std::ifstream infile("amits_example.csv");  // make a check about the file
    DataFrame df1 = DataFrame(infile);
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    std::cout << DataFrame(df1["total_sales_volume"]);
    df1.fill_na<double>("total_sales_volume", 0);
    DataFrame test = DataFrame(df1["total_sales_volume"] > 0);
    df1["is_sale"] = test;
    df1["is_sale"] = df1["total_sales_volume"] > 0.;
    vector<string> cols = get_columns();
    DataFrame df2 = DataFrame(df1[cols]);
    df2.dropna();
    std::cout << df2.size().first << std::endl;
    std::cout << df2.size().second << std::endl;
    vector<double> series;
    DataFrame::const_iter<double> real_begin =
        df2.cbegin<double>("total_sales_volume");
    DataFrame::const_iter<double> real_end =
        df2.cend<double>("total_sales_volume");
    DataFrame::const_iter<double> predicitons_begin =
        df2.cbegin<double>("saex2_initial_actual");
    auto fun = [](const double& a, const double& b) { return std::abs(a - b); };
    std::transform(real_begin, real_end, predicitons_begin,
                   std::back_inserter(series), fun);
    double avg =
        std::accumulate(series.begin(), series.end(), 0.) / series.size();
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    std::cout << avg << std::endl;
    auto duration = duration_cast<milliseconds>(t2 - t1).count();
    std::cout << "The entire duration is: " << duration << std::endl;
    return 0;
}
