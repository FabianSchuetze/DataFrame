#include "dataframe/dataframe.h"
#include "dataframe/dataframeproxy.h"
#include "dataframe/ColumnIterator.h"
//#include "dataframe/ConstColumnIterator.h"
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <cmath>
#include <chrono>

using std::vector;
using std::string;


void fun(DataFrame& df) {
    vector<string> string_col = {"new_test", "second"};
    df["test"] = string_col;
}

// INTERESTING: DIDN'T DEFINE ASSIGNMENT OPERATOR BUT DOES WHAT I WANT, HOW?
void fun2(DataFrame& df) {
    vector<string> string_col = {"ttt", "ssss"};
    df["test"] = string_col;
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
    using milli = std::chrono::milliseconds;
    std::ifstream infile("test2.csv"); //make a check about the file
    DataFrame df1 = DataFrame(infile);
    auto start = std::chrono::high_resolution_clock::now();
    //DataFrame df2 = df1;
    df1.dropna();
    df1.sort_by_column("account_id");
    auto finish = std::chrono::high_resolution_clock::now();
    auto total =std::chrono::duration_cast<milli>(finish - start).count();
    std::cout << "myFunction() took " << total << " milliseconds\n";
    std::cout << df1.size().first << std::endl;
    std::cout << df1.size().second << std::endl;
    ////std::cout << df1.loc("0") << std::endl;
    std::cout << df1 << std::endl;
    ////std::cout << "trying to get the new data\n";
    //DataFrame df2 = DataFrame(df1["total_sales_volume"]);
    //std::cout << df2 <<std::endl;
    return 0;
}
