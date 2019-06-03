#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include "dataframe.h"

using std::make_shared;
using std::string;
using std::transform;
using std::vector;

DataFrame::DataFrameProxy DataFrame::operator[](const string& col_name) {
    return DataFrameProxy(*this, col_name);
}

DataFrame::DataFrameProxy DataFrame::operator[](const vector<string>& col_names) {
    return DataFrameProxy(*this, col_names);
}

DataFrame& DataFrame::operator=(const DataFrame& rhs) {
    if (this != &rhs) {
        columns = rhs.columns;
        column_names = rhs.column_names;
    }
    return *this;
}

bool find_or_add(const string& name, std::map<string, int>& columns) {
    try {
        columns.at(name);
    } catch (const std::out_of_range& e) {
        columns[name] = columns.size();
    }
    return columns.at(name);
}

template <typename T>
void DataFrame::DataFrameProxy::add_or_replace(bool replace, int idx,
                                               const vector<T>& data) {
    if (replace) {
        if (theDataFrame.columns[idx].use_count() > 1) {
            std::cout << "copy-on-write\n";
            theDataFrame.columns.at(idx) = make_shared<Column>(data);
        } else {
            *theDataFrame.columns[idx] = data;
        }
    } else
        theDataFrame.columns.push_back(make_shared<Column>(data));
}

void DataFrame::make_unique_if(const std::string& s) {
    if (this->use_count(s) > 1) {
        std::cout << "copy-on-write\n";
        int idx = column_names[s];
        std::shared_ptr<Column> data = make_shared<Column>(*columns.at(idx));
        columns.at(idx) = data;
    }
}

void DataFrame::DataFrameProxy::add_or_replace(
    bool replace, int idx, const std::shared_ptr<Column>& data) {
    if (replace) {
        if (theDataFrame.columns[idx].use_count() > 1) {
            theDataFrame.columns.at(idx) = data;
        } else {
            theDataFrame.columns[idx] = data;
        }
    } else
        theDataFrame.columns.push_back(data);
}

void DataFrame::DataFrameProxy::check_column_size(size_t check) {
    if (colNames.size() != check)
        throw std::invalid_argument("different number of columns");
}

template <typename T>
void DataFrame::DataFrameProxy::insert_column(const string& name,
                                              const vector<T>& inp) {
    int capacity_so_far = theDataFrame.column_names.size();
    int lhsIndex = find_or_add(name, theDataFrame.column_names);
    bool replace = theDataFrame.column_names.size() == capacity_so_far;
    add_or_replace(replace, lhsIndex, inp);
}

DataFrame::DataFrameProxy& DataFrame::DataFrameProxy::operator=(
    const DataFrameProxy& rhs) {
    check_column_size(rhs.colNames.size());
    for (size_t i = 0; i < colNames.size(); ++i) {
        int capacity_so_far = theDataFrame.column_names.size();
        int lhsIndex = find_or_add(colNames[i], theDataFrame.column_names);
        int rhsIndex = rhs.theDataFrame.column_names.at(rhs.colNames[i]);
        bool replace = theDataFrame.column_names.size() == capacity_so_far;
        add_or_replace(replace, lhsIndex, rhs.theDataFrame.columns[rhsIndex]);
    }
    return *this;
}

DataFrame::DataFrameProxy& DataFrame::DataFrameProxy::operator=(
    const vector<vector<double>>& other_col) {
    check_column_size(other_col.size());
    for (size_t i = 0; i < colNames.size(); ++i)
        insert_column(colNames[i], other_col[i]);
    return *this;
}

template <typename T>
DataFrame::DataFrameProxy& DataFrame::DataFrameProxy::operator=(
    const vector<T>& other_col) {
    check_column_size(1);
    insert_column(colNames[0], other_col);
    return *this;
}
template DataFrame::DataFrameProxy& DataFrame::DataFrameProxy::operator=(
    const vector<double>& other_col);
template DataFrame::DataFrameProxy& DataFrame::DataFrameProxy::operator=(
    const vector<string>& other_col);

void append_string(Column& c, std::string& s, int pos) {
    c.append_string(s, pos);
    s += ' ';
}

std::ostream& operator<<(std::ostream& os,
                         const DataFrame::DataFrameProxy& df) {
    string output;
    for (string const& x : df.colNames) output += x + ' ';
    output += '\n';
    for (int row = 0; row < df.theDataFrame.size().first; ++row) {
        for (string const& col : df.colNames) {
            int col_number = df.theDataFrame.column_names[col];
            append_string(*df.theDataFrame.columns[col_number], output, row);
        }
        output += '\n';
    }
    os << output;
    return os;
}

std::ostream& operator<<(std::ostream& os, const DataFrame& df) {
    string output;
    for (auto const& x : df.column_names) output += x.first + ' ';
    output += '\n';
    for (int row = 0; row < df.size().first; ++row) {
        for (auto const& col : df.column_names)
            append_string(*df.columns[col.second], output, row);
        output += '\n';
    }
    os << output;
    return os;
}

DataFrame& DataFrame::operator+=(const DataFrame& rhs) {
    for (auto& x : column_names) {
        make_unique_if(x.first);
        try {
            int rhsIdx = rhs.column_names.at(x.first);
            *columns[x.second] += *rhs.columns[rhsIdx];
        } catch (const std::out_of_range& e) {
            columns[x.second]->replace_nan();
        }
    }
    return *this;
}

DataFrame operator+(const DataFrame& lhs, const DataFrame& rhs) {
    DataFrame sum = lhs;
    sum += rhs;
    return sum;
}
