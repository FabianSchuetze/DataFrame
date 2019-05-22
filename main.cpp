//#include "dataframe/Column.h"
#include "dataframe/dataframe.h"
#include <vector>
#include <string>
#include <iostream>
#include <utility>
#include <map>
#include <memory>
using std::vector;
using std::string;
using std::map;

int main() {
    vector<double> res = {2,3, 4};
    //string name1 = "first_col";
    vector<double> res2 = {20,30, 40};
    vector<vector<double>> cols = {res, res2};
    vector<string> names = {"first_col", "second_col"};
    //string name2 = "second_col";
    DataFrame df;
    DataFrame df2 = DataFrame(names, cols);
    std::cout << cols[0].size() << std::endl;
    DataFrame df3 = DataFrame(names, std::move(cols));
    std::cout << cols[0].size() << std::endl;

    std::cout << "The use_count for first_col is: " << 
        df3.use_count("first_col") << std::endl;
    DataFrame df4 = df3;
    std::cout << "The use_count for first_col is: " << 
        df3.use_count("first_col") << std::endl;

    DataFrame df5 = std::move(df3);
    df5.insert("test", std::move(res));
    std::cout << "The use_count for first_col is and test: " << 
        df5.use_count("first_col") << ", " <<
        df5.use_count("test") << std::endl;
    std::cout << res.size() << std::endl;

    //Column col1 = Column(name, res);
    //std::cout << res[0] << std::endl;
    //Column col2 = Column(name, std::move(res));
    //std::cout << res.size() << std::endl;
    //columns.push_back(std::make_shared<Column>(col1));
    ////columns2(columns);
    return 0;
}
