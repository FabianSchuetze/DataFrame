#include "dataframe/dataframe.h"
#include "dataframe/dataframeproxy.h"
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
    //DataFrame df1 = df2;
    //df2 += df2;
    std::cout << df2 << std::endl;
    std::vector<double>::iterator it = df2.begin<double>("third_col");
    std::vector<double>::iterator end = df2.end<double>("third_col");
    vector<double> res;
    std::transform(it, end, std::back_inserter(res),
            [](const double& i) -> double {return i > 0 ? 1 : 0;}); 
    for (auto const& i : res) std::cout << i;
    std::cout << std::endl;
    df2["is_sale"] = res;
    std::cout << df2 << std::endl;
    std::cout << df2["is_sale"] << std::endl;
    std::vector<double>::iterator it2 = df2.begin<double>("third_col");
    std::vector<double>::iterator end2 = df2.end<double>("third_col");
    while (it2 != end2) {
        std::cout << *it2 << " ";
        it2++;
    }
    std::cout << std::endl;
    //std::cout << df1 << std::endl;
    return 0;
}
