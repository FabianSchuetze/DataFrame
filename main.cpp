#include "dataframe/dataframe.h"
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

int main() {
    vector<vector<double>> first = {{10, 20}, {30, 40}};
    vector<vector<double>> second = {{-10, -20}, {-30, -40}, {-100, 6}};
    vector<vector<string>> strings = {{"f", "l"}, {"m", "a"}};
    vector<string> string_col = {"new_test", "second"};
    vector<double> tmp = {1120};
    vector<string> col_names = {"first_col", "second_col"};
    vector<string> idx_names = {"1", "2"};
    vector<string> idx_names2 = {"3", "1"};
    vector<string> long_names = {"first_col", "second_col", "third_col"};
    vector<string> lhs_names = {"first_col", "second_col"};
    vector<string> rhs_names = {"first_col", "third_col"};
    DataFrame df2 = DataFrame(idx_names2, col_names, strings);
    DataFrame df1 = DataFrame(idx_names, col_names, strings);
    std::cout << df1 << std::endl;
    std::cout << df2 << std::endl;
    df1 += df2;
    std::cout << df1 << std::endl;
    //df1["first_col"] = tmp;
    //std::cout << df1 << std::endl;
    return 0;
}
