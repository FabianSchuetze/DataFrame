#include "dataframe/dataframe.h"
#include "dataframe/dataframeproxy.h"
#include "dataframe/ColumnIterator.h"
#include "dataframe/ConstColumnIterator.h"
#include <vector>
#include <fstream>
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
    df.sort_by_column(s);
}

// NEED TO THINK HOW I CAN DO THIS WTTH CMAKE!
int main() {
    std::ifstream infile("forecast2.csv"); //make a check about the file
    DataFrame df1 = DataFrame(infile);
    std::cout << "completed\n";
    std::cout << df1.size().first << std::endl;
    std::cout << df1.size().second << std::endl;
    std::cout << df1.loc("2019-05-01") << std::endl;
    DataFrame df2 = DataFrame(df1.loc("2019-04-30"));
    std::cout << df1 << std::endl;
    df1.sort_by_index();
    std::cout << df1 << std::endl;
    df1.sort_by_column("saex2");
    std::cout << df1 << std::endl;
    return 0;
}
