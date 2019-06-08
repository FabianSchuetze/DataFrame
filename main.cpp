#include "dataframe/dataframe.h"
#include "dataframe/dataframeproxy.h"
#include "dataframe/ColumnIterator.h"
#include "dataframe/ConstColumnIterator.h"
#include <vector>
#include <string>
#include <iostream>

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
    DataFrame::iter<double> it = df.begin<double>("third_col");
    DataFrame::iter<double>  e = df.end<double>("third_col");
    DataFrame::const_iter<double> it2 = df.cbegin<double>("third_col");
    DataFrame::const_iter<double>  e2 = df.cend<double>("third_col");
    std::fill(it, e, 10);
}

int main() {
    typedef std::numeric_limits<double> nan;
    double d = nan::quiet_NaN();
    vector<vector<double>> first = {{10, d, 20, d}, {30, d, 40, d}};
    vector<vector<double>> second = {{-10, -20}, {-30, -40}, {-100, 6}};
    vector<vector<string>> strings2 = {{"f", "l"}, {"m", "a"}, {"as", "ssd"}};
    vector<string> string_col = {"new_test", "second"};
    vector<string> col_names = {"first_col", "second_col"};
    vector<string> col_names2 = {"first_col", "second_col", "third_col"};
    vector<string> idx_names = {"1", "2", "4", "5"};
    vector<string> idx_names2 = {"3", "1"};
    DataFrame df2 = DataFrame(idx_names, col_names, first);
    DataFrame df1 = df2;
    //std::cout << df1["first_col"] + df1["first_col"];
    //vector<double> res = {49, 29};
    std::cout << df2 << std::endl;
    df2.reorder_index();
    df2.dropna();
    std::cout << df2 << std::endl;
    //fill_df(df2);
    //// THIS CANNOT BE DONE YET!!!!
    ////df2.loc("2") = res;
    //std::cout << df2 << std::endl;
    return 0;
}
