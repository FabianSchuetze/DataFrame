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
        DataFrame::iter<double> it = df.begin<double>(s);
        DataFrame::iter<double>  e = df.end<double>(s);
        std::transform(it, e, it, [](auto& d) {return std::isnan(d) ? 0 : d;});
    }
}

//template <typename T, typename T2...>
//void sort_df(DataFrame& df, const std::string& s) {
    //df.sort_by_column<T>(df.cbegin<T>(s), df.cbegin<T>(s));
//}

int main() {
    vector<vector<double>> first = {{10, 10, -10, -8}, {30, -100, 40, 100}};
    vector<vector<double>> second = {{-10, -20, -30, -40}, {-100, 6, 0, 0}};
    vector<vector<string>> strings2 = {{"f", "l"}, {"m", "a"}, {"as", "ssd"}};
    vector<string> string_col = {"u ", "NA", "NA", "second"};
    vector<string> col_names = {"first_col", "second_col"};
    vector<string> col_names2 = {"first_col", "second_col", "third_col"};
    vector<string> idx_names = {"1", "2", "4", "5"};
    vector<string> idx_names2 = {"3", "1"};
    DataFrame df2 = DataFrame(idx_names, col_names, first);
    DataFrame df1 = DataFrame(idx_names, col_names, second);
    df2["test_col"] = string_col;
    std::cout << df2 << std::endl;
    df2.sort_by_column<string, string>(df2.cbegin<string>("test_col"),
                                       df2.cbegin<string>("test_col"));
    //sort_df<string>(df2, "test_col");
    //df2 += 2;
    //std::cout << df1 << std::endl;
    //std::cout << df2 << std::endl;
    ////std::cout << "is contigious: " << df2.is_contigious() << std::endl;
    //vector<string> sub({"first_col", "second_col"});
    //DataFrame abc = df1[sub];
    //DataFrame::Grouper<double> group = df1[sub].groupby<double>("second_col");
    //DataFrame::Grouper<double> group = abc.groupby<double>("second_col");
    //DataFrame::Grouper<double> group2 = df2.groupby<double>("first_col");
    // NAMESPACE POLUTION!!
    //mean t;
    //Statistic *p = &t;
    //mean t;
    //p = &t;
    //DataFrame test = group.summarize(p);
    //DataFrame test2 = group2.summarize(p);
    //DataFrame test2 = group.summarize(&DataFrame::Grouper<double>::mean)
        //group->*f);
    //std::cout << test << std::endl;
    //std::cout << test2 << std::endl;
    return 0;
}
