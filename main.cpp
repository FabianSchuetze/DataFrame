#include "dataframe/dataframe.h"
#include "dataframe/dataframeproxy.h"
#include "dataframe/ColumnIterator.h"
#include "dataframe/ConstColumnIterator.h"
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
        DataFrame::iter<double> it = df.begin<double>(s);
        DataFrame::iter<double>  e = df.end<double>(s);
        std::transform(it, e, it, [](auto& d) {return std::isnan(d) ? 0 : d;});
    }
}

template <typename T>
void sort_df(DataFrame& df, const std::string& s) {
    df.sort_by_column<T>(s);
}

int main() {
    vector<vector<bool>> first = {{true, false}, {false, true}};
    vector<vector<double>> second = {{1, 6}, {-1, 10}};
    vector<std::string> string_col = {"fa", "ma"};
    vector<double> bool_col = {false, true};
    vector<string> idx_names = {"1", "2"};
    vector<string> col_names = {"first_col", "second_col"};
    DataFrame df1 = DataFrame(idx_names, col_names, first);
    DataFrame df2 = DataFrame(idx_names, col_names, second);
    std::cout << df2;
    std::cout << df1;
    df1["test"] = df2["second_col"] < 80.;
    //std::cout << test;
    //df1.convert_bool_to_double("first_col");
    std::cout << df1;

    //DataFrame df1 = df2;
    //df2["test_col2"] = bool_col;
    //std::cout << df2 << std::endl;
    //std::cout << df1 << std::endl;
    //std::cout << df1 + df2;
    //std::cout << df2["second_col"] << std::endl;
    //sort_df<string>(df2, "test_col");
    //df2 += 2;
    //std::cout << df2 << std::endl;
    //std::cout << std::endl;
    //std::cout << "is contigious: " << df2.is_contigious() << std::endl;
    //df2.make_contigious();
    ////DataFrame df3 = deep_copy(df2);
    ////for (const auto& i : df3.get_index_positions())
        ////std::cout << i << std::endl;
    //std::cout << df2 << std::endl;
    //std::cout << df1 << std::endl;
    //std::cout << "is contigious: " << df2.is_contigious() << std::endl;
    return 0;
}
