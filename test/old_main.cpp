#include "../dataframe/dataframe.h"
#include "../dataframe/dataframeproxy.h"
#include "../dataframe/ColumnIterator.h"
#include "../dataframe/ConstColumnIterator.h"
#include "../dataframe/grouper.h"
#include <vector>
#include <string>
#include <iostream>
#include <cmath>

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

//template <typename T>
//void sort_df(DataFrame& df, const std::string& s) {
    //df.sort_by_column(s);
//}

int main() {
    vector<vector<string>> strings2 = {{"f", "l"}, {"m", "a"}, {"as", "ssd"}};
    vector<double> first1 = {30, -100, 40, 100};
    vector<string> string_col = {"u ", "NA", "new_test", "second"};
    vector<string> col_names = {"first_col", "second_col"};
    vector<string> col_names2 = {"first_col", "second_col", "third_col"};
    vector<string> idx_names = {"1", "2", "4", "5"};
    vector<string> idx_names2 = {"3", "1"};
    DataFrame df2 = DataFrame(Index(idx_names), col_names, first1);
    DataFrame df1 = df2;
    df2["test_col"] = string_col;
    std::cout << df2 << std::endl;
    //sort_df<string>(df2, "test_col");
    //df2 += 2;
    std::cout << df2 << std::endl;
    std::cout << "is contigious: " << df2.is_contigious() << std::endl;
    df2.make_contigious();
    std::cout << df2 << std::endl;
    std::cout << df1 << std::endl;
    std::cout << "is contigious: " << df2.is_contigious() << std::endl;
    //DataFrame::Grouper<double> group = df2.groupby<double>("test_col");
    return 0;
}
