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
    vector<vector<double>> first = {{10, 20}, {30, 40}};
    vector<vector<double>> second = {{-10, -20}, {-30, -40}, {-100, 6}};
    vector<vector<string>> strings = {{"f", "l"}, {"m", "a"}};
    vector<vector<string>> strings2 = {{"f", "l"}, {"m", "a"}, {"as", "ssd"}};
    vector<string> string_col = {"new_test", "second"};
    vector<double> tmp = {1120};
    vector<string> col_names = {"first_col", "second_col"};
    vector<string> col_names2 = {"first_col", "second_col", "third_col"};
    vector<string> idx_names = {"1", "2"};
    vector<string> idx_names2 = {"3", "1"};
    vector<string> long_names = {"first_col", "second_col", "third_col"};
    vector<string> lhs_names = {"first_col", "second_col"};
    vector<string> rhs_names = {"first_col", "third_col"};
    DataFrame df2 = DataFrame(idx_names2, col_names2, second);
    DataFrame df1 = df2;
    vector<double> res = {49, 29};
    std::cout << df2 << std::endl;
    df2.reorder_index();
    fill_df(df2);
    // THIS CANNOT BE DONE YET!!!!
    //df2.loc("2") = res;
    std::cout << df2 << std::endl;
    return 0;
}
