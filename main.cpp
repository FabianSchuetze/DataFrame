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
    vector<double> tmp = {1120, 10};
    vector<string> names = {"first_col", "second_col"};
    vector<string> long_names = {"first_col", "second_col", "third_col"};
    vector<string> lhs_names = {"first_col", "second_col"};
    vector<string> rhs_names = {"first_col", "third_col"};
    //DataFrame df1 = DataFrame(names, strings);
    DataFrame df1 = DataFrame(names, first);
    //std::cout << df1 << std::endl;
    ////df1["test"] = names;
    //DataFrame dfPlus = df1 + "fabi";
    //std::cout << dfPlus;
    //std::cout << dfPlus.use_count("first_col");
    DataFrame df2 = DataFrame(long_names, second); //must add an alerat about the same length
    std::cout << df1 << std::endl;
    fun(df1);
    //DataFrame df2 = df1;
    std::cout << df1 << std::endl;
    fun2(df1);
    std::cout << df1 << std::endl;
    df1["test"] = string_col;
    std::cout << df1 << std::endl;
    df2["first_col"];
    df1["fabi"] = df2["first_col"];
    std::cout << "for fabi: " << df1.use_count("fabi") << std::endl;
    std::cout << "for first_col: " << df2.use_count("first_col") << std::endl;
    std::cout << "for second_col: " << df2.use_count("second_col") << std::endl;
    std::cout << df2 << std::endl;
    df1["fabi"] = df2["second_col"];
    std::cout << df2 << std::endl;
    std::cout << df1 << std::endl;
    df1["fabi"] = tmp;
    std::cout << "for fabi: " << df1.use_count("fabi") << std::endl;
    std::cout << "for first_col: " << df2.use_count("first_col") << std::endl;
    std::cout << "for second_col: " << df2.use_count("second_col") << std::endl;
    std::cout << df2 << std::endl;
    std::cout << df1 << std::endl;
    //df1["fist_col"] = tmp;
    DataFrame test = df1["first_col"];
    std::cout << "Before the addition\n";
    std::cout << df1 << std::endl;
    std::cout << df2 << std::endl;
    std::cout << "for fabi: " << df1.use_count("fabi") << std::endl;
    std::cout << "for first_col: " << df1.use_count("first_col") << std::endl;
    std::cout << "for second_col: " << df1.use_count("second_col") << std::endl;
    df1 += df1;
    std::cout << "After the addition\n";
    std::cout << df1 << std::endl;
    std::cout << df2 << std::endl;
    std::cout << test << std::endl;
    DataFrame summation = df1 + df2;
    std::cout << summation << std::endl;
    //std::vector<string>::iterator b = df1.begin<string>("test");
    //std::cout << *b << std::endl;
    return 0;
}
