#include "dataframe.h"
#include <iostream>
#include <memory>
#include <utility>
#include <stdexcept>
#include "Column.h"

using std::string;
using std::vector;
using std::make_shared;

DataFrame::DataFrame(const vector<string>& names,
                     const vector<vector<double>>& cols) {
    // check if both vectors have the same length;
    for (size_t i = 0; i < names.size(); ++i) {
        columns.push_back(make_shared<Column>(Column(names[i], cols[i])));
        column_names[names[i]] = i;
    }
}
DataFrame::DataFrame(const vector<string>& names,
                     vector<vector<double>>&& cols) {
    // check if both vectors have the same length;
    for (size_t i = 0; i < names.size(); ++i) {
        columns.push_back(
            make_shared<Column>(Column(names[i], std::move(cols[i]))));
        column_names[names[i]] = i;
    }
}

// Move and Copy (Assignment) Constructor
DataFrame::DataFrame(DataFrame& df)
    : columns(df.columns), column_names(df.column_names) {}

DataFrame& DataFrame::operator=(DataFrame& rhs) {
    if (this != &rhs) {
        columns = rhs.columns;
        column_names = rhs.column_names;
    }
    return *this;
}

DataFrame::DataFrame(DataFrame&& df) {
    columns = std::move(df.columns);
    column_names = std::move(df.column_names);
}

DataFrame& DataFrame::operator=(DataFrame&& rhs) {
    if (this != &rhs) {
        columns = std::move(rhs.columns);
        column_names = std::move(rhs.column_names);
    }
    return *this;
}

int DataFrame::use_count(string col_name) {
    return columns[column_names[col_name]].use_count();
}

void DataFrame::insert(const string& n, const vector<double>& vec) {
    columns.push_back(make_shared<Column>(Column(n, vec)));
    int previous_size = column_names.size();
    column_names[n] = previous_size;
}

void DataFrame::insert(const string& n, vector<double>&& vec) {
    columns.push_back(make_shared<Column>(Column(n, std::move(vec))));
    int previous_size = column_names.size();
    column_names[n] = previous_size;
}
