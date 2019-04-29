#include "dataframe.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>

using std::find;
using std::string;
using std::vector;
using std::map;

// I think I don't need the default constructor, do I?  DataFrame();

void DataFrame::init_map(const vector<string> container,
                         map<string, int>& dict) {
    vector<string>::const_iterator iter = container.begin();
    int val = 0;
    for (; iter != container.end(); ++iter) {
        dict[*iter] = val++;
    }
}

DataFrame::DataFrame(const vector<string>& idx) {
    init_map(idx, index);
}
    //vector<string>::const_iterator iter = idx.begin();
    //int val = 0;
    //for (; iter != idx.end(); ++iter) {
        //index[*iter] = val++;
    //}
//}
    //index = idx; }

DataFrame::DataFrame(const vector<string>& idx, const vector<string>& cols,
                     const vector<vector<double>>& datainp) {
    init_map(idx, index);
    init_map(cols, columns);
    vector<data_col>::const_iterator iter = datainp.begin();
    vector<data_col* > ttest;
    for (;iter != datainp.end(); ++iter) {
        ttest.push_back(iter);
        data.push_back(iter);
        data++;
    }
}

const std::vector<double> DataFrame::operator()(const std::string& s) {
    string_iter pos = find_index(index.begin(), index.end(), s);
    vector<string>::size_type row_number = pos - index.begin();
    vector<vector<double>>::const_iterator it = columns.begin();
    vector<double> results;
    for (; it != columns.end(); ++it) results.push_back((*it)[row_number]);
    return results;
}

DataFrame::string_iter DataFrame::find_index(const string_iter b,
                                             const string_iter e,
                                             const std::string& s) {
    string_iter pos = find(b, e, s);
    if (pos == e) throw std::domain_error("Index s is not present");
    return pos;
}

double DataFrame::operator()(const std::string& time, const std::string& col) {
    string_iter pos_row = find_index(index.begin(), index.end(), time);
    string_iter pos_col = find_index(names.begin(), names.end(), col);
    vector<string>::size_type row_number = pos_row - index.begin();
    vector<string>::size_type col_number = pos_col - names.begin();
    vector<double> results;
    return columns[row_number][col_number];
}
