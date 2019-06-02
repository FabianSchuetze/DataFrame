#include "dataframe.h"
#include <iostream>
#include <ostream>
#include <stdexcept>
using std::make_shared;
using std::string;
using std::vector;

DataFrame::DataFrame(const vector<string>& n, const vector<vector<double>>& cols) {
    if (n.size() != cols.size())
        throw std::invalid_argument("Column name len does not equal col len");
    for (size_t i = 0; i < n.size(); ++i) {
        columns.push_back(make_shared<Column>(cols[i]));
        column_names[n[i]] = i;
    }
}

DataFrame::DataFrame(const vector<string>& n, const vector<vector<string>>& cols) {
    if (n.size() != cols.size())
        throw std::invalid_argument("Column name len does not equal col len");
    for (size_t i = 0; i < n.size(); ++i) {
        columns.push_back(make_shared<Column>(cols[i]));
        column_names[n[i]] = i;
    }
}

DataFrame::DataFrame(const DataFrame::DataFrameProxy df)
{
    int i = 0;
    for (string name: df.colNames) {
        int pos = df.theDataFrame.column_names[name];
        columns.push_back(df.theDataFrame.columns[pos]);
        column_names[name] = i;
        i++;
    }
}

DataFrame::DataFrameProxy::DataFrameProxy(DataFrame& df, const string& s)
    : theDataFrame(df), colNames{s} {};

DataFrame::DataFrameProxy::DataFrameProxy(DataFrame& df, const vector<string>& s)
    : theDataFrame(df), colNames(s){};

const std::pair<int, int> DataFrame::size() const {
    std::pair<int, int> size;
    size.first = columns[0]->size();
    size.second = columns.size();
    return size;
}

const int DataFrame::use_count(string col) {
    return columns[column_names[col]].use_count();
}
