#include "dataframe/dataframe.h"
#include "dataframe/dataframeproxy.h"
#include "dataframe/ColumnIterator.h"
#include "dataframe/ConstColumnIterator.h"
#include "dataframe/grouper.h"
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <numeric>

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
        DataFrame::iterator<double> it = df.begin<double>(s);
        DataFrame::iterator<double>  e = df.end<double>(s);
        std::transform(it, e, it, [](auto& d) {return std::isnan(d) ? 0 : d;});
    }
}

template <typename T>
void sort_df(DataFrame& df, const std::string& s) {
    df.sort_by_column<T>(df.cbegin<T>(s), df.cbegin<T>(s));
}

int main() {
    vector<vector<double>> first = {{10, 10, 10, -8}, {30, -100, 40, 100}};
    vector<vector<double>> second = {{-10, -20, -30, -40}, {-100, 6, 0, 0}};
    vector<vector<string>> strings2 = {{"f", "l"}, {"m", "a"}, {"as", "ssd"}};
    vector<string> string_col = {"u", "u", "z", "second"};
    vector<string> col_names = {"first_col", "second_col"};
    vector<string> col_names2 = {"first_col", "second_col", "third_col"};
    vector<string> idx_names = {"1", "2", "4", "5"};
    vector<string> idx_names2 = {"3", "1"};
    DataFrame df2 = DataFrame(idx_names, col_names, first);
    DataFrame df1 = DataFrame(idx_names, col_names, second);
    df2["test_col"] = string_col;
    std::cout << df2 << std::endl;
    DataFrame::const_iterator<double> b = df2.cbegin<double>("first_col");
    DataFrame::const_iterator<double> a = df2.cend<double>("first_col");
    std::cout << df2 << std::endl;
    DataFrame::Grouper<double, string> group2 = df2.groupby<double, string>(
            df2.cbegin<double>("first_col"),
            df2.cbegin<string>("test_col"));
            //df2.cbegin<double>("second_col"));
    ////// NAMESPACE POLUTION!!
    mean t;
    Statistic *p = &t;
    DataFrame test2 = group2.summarize(p);
    std::cout << test2 << std::endl;
    std::cout << df2.use_count("first_col") << std::endl;
    DataFrame::iterator<double> itb = df2.begin<double>("first_col");
    DataFrame::iterator<double> ite = df2.end<double>("first_col");
    vector<double>res(itb, ite);
    std::replace(itb, ite, 10, 100);
    std::cout << df2 << std::endl;

    //if (it != ite)
        //std::cout << *it << std::endl;
    //else
        //std::cout << "not found\n";
    //return 0;
}
