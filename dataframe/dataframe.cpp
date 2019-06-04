#include "dataframe.h"
#include <iostream>
#include <ostream>
#include <stdexcept>
using std::make_shared;
using std::string;
using std::vector;
using std::make_pair;
using std::pair;


void DataFrame::get_index_names(vector<string>& inp) {
    for (auto const& idx_pair : index_names)
        inp.push_back(idx_pair.first);
}

template <typename T>
DataFrame::DataFrame(const vector<string>& idx, const vector<string>& n,
                     const vector<vector<T>>& cols) {
    if (n.size() != cols.size())
        throw std::invalid_argument("Column name len does not equal col len");
    for (size_t i = 0; i < n.size(); ++i) {
        if (idx.size() != cols[i].size())
            throw std::invalid_argument("Data and index has different length");
        columns.push_back(make_shared<Column>(cols[i]));
        column_names[n[i]] = i;
    }
    for (size_t i = 0; i < idx.size(); ++i)
        index_names.push_back(make_pair(idx[i], i));
}
template DataFrame::DataFrame(const vector<string>&, const vector<string>&,
                              const vector<vector<double>>&);
template DataFrame::DataFrame(const vector<string>&, const vector<string>&,
                              const vector<vector<string>>&);

DataFrame::DataFrame(const DataFrame::DataFrameProxy& df) {
    int i = 0;
    for (string name : df.colNames) {
        int pos = df.theDataFrame.column_names[name];
        columns.push_back(df.theDataFrame.columns[pos]);
        column_names[name] = i;
        i++;
    }
}

DataFrame::DataFrameProxy::DataFrameProxy(DataFrame& df, const vector<string>&i, 
                                          const string& s)
    : theDataFrame(df), idxNames{i}, colNames{s} {};

DataFrame::DataFrameProxy::DataFrameProxy(DataFrame& df,
                                          const vector<string>& i,
                                          const vector<string>& s)
    : theDataFrame(df), idxNames{i}, colNames(s){};

const std::pair<int, int> DataFrame::size() const {
    std::pair<int, int> size;
    size.first = columns[0]->size();
    size.second = columns.size();
    return size;
}

const int DataFrame::use_count(string col) {
    return columns[column_names[col]].use_count();
}

const int find_position(const string& name, 
                        const vector<pair<string, int>>& vec) {
    int tmp = -1;
    for (const auto& x: vec) {
        if (x.first == name) {
            tmp = x.second;
            break;
        }
    }
    return tmp;
}

const string find_name(const int& i, const vector<pair<string, int>>& vec) {
    string tmp = "NA";
    for (const auto& x: vec) {
        if (x.second == i) {
            tmp = x.first;
            break;
        }
    }
    return tmp;
}

vector<int> correspondence_position(const DataFrame& lhs, 
                                    const DataFrame& other) {
    vector<int> res;
    for (size_t i = 0; i < lhs.index_names.size(); ++i) {
        const string lhs_name = find_name(i, lhs.index_names);
        res.push_back(find_position(lhs_name, other.index_names));
    }
    return res;
}
