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
#include <random>

using std::vector;
using std::string;

vector<double> return_random_numbers(int size, int seed) {
    std::default_random_engine e(seed);
    std::normal_distribution<> n(0, 1);
    vector<double> res;
    for (int i = 0; i < size; i++)
        res.push_back(n(e));
    return res;
}
int main() {
    vector<string> key1 = {"a", "a", "b", "b", "a"};
    vector<string> key2 = {"one", "two", "one", "two", "one"};
    vector<double> data1 = return_random_numbers(5, 0);
    vector<double> data2 = return_random_numbers(5, 10);
    DataFrame df = DataFrame({"key1", "key2", "data1", "data2"},
                             key1, key2, data1, data2);
    std::cout << df << std::endl;
    DataFrame df2 = DataFrame({"key1"}, key1);
    std::cout << df2 << std::endl;
    DataFrame::Grouper<string> grouper =
        df.groupby<string>(df.cbegin<string>("key1"));
    //DataFrame::Grouper<string> grouper2 = df.groupby<string>(df2);
    count t;
    Statistic *p = &t;
    DataFrame works = grouper.summarize(p, {"data1"});
    DataFrame works2 = grouper.summarize(p);
    DataFrame test = df.summarize(p);
    std::cout << test << std::endl;
    std::cout <<  grouper.summarize(p, {"data1"}) << std::endl;
    std::cout <<  grouper.summarize(p) << std::endl;
    std::cout << df.summarize(p) << std::endl;
}
