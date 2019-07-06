#include "../dataframe/dataframe.h"
#include "../dataframe/dataframeproxy.h"
#include "../dataframe/ColumnIterator.h"
#include "../dataframe/ConstColumnIterator.h"
#include "../dataframe/grouper.h"
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <numeric>

using std::vector;
using std::string;
int main() {
    vector<vector<double>> first = {{10, 10, -10, -8}, {30, -100, 40, 100}};
    vector<int> idx1 = {-10, -20, -30, -40};
    vector<string> idx2 = {"1", "2", "4", "5"};
    Index idx = Index(idx1, idx2);
    vector<string> col_names = {"first_col", "second_col"};
    DataFrame df1 = DataFrame(idx, col_names, first);
    std::cout << df1 << std::endl;
}
