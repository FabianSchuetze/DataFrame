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
DataFrame::DataFrameProxy DataFrame::operator[](string col_name) {
    return DataFrameProxy(*this, col_name);
}

DataFrame::DataFrameProxy DataFrame::operator[](vector<string> col_names) {
    return DataFrameProxy(*this, col_names);
}

int find_or_add(string name, std::map<string, int>& columns) {
    int idx;
    try {
        idx = columns.at(name);
    } catch (const std::out_of_range& e) {
        idx = columns.size();
        columns[name] = idx;
    }
    return idx;
}

void DataFrame::DataFrameProxy::add_or_replace(bool replace, int idx,
                                               vector<double>& data) {
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

void DataFrame::DataFrameProxy::add_or_replace(bool replace, int idx,
                                               vector<string>& data) {
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

void DataFrame::DataFrameProxy::add_or_replace(
    bool replace, int idx, std::shared_ptr<Column> data) {
    if (replace) {
        if (theDataFrame.columns[idx].use_count() > 1) {
            theDataFrame.columns.at(idx) = data;
        } else {
            theDataFrame.columns[idx] = data;
        }
    } else
        theDataFrame.columns.push_back(data);
}

DataFrame::DataFrameProxy& DataFrame::DataFrameProxy::operator=(
    const DataFrameProxy& rhs) {
    if (colNames.size() != rhs.colNames.size())
        throw std::invalid_argument("different number of columns");
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
    vector<vector<double>>& other_col) {
    if (colNames.size() != other_col.size())
        throw std::invalid_argument("different number of columns");
    for (size_t i = 0; i < colNames.size(); ++i) {
        int capacity_so_far = theDataFrame.column_names.size();
        int lhsIndex = find_or_add(colNames[i], theDataFrame.column_names);
        bool replace = theDataFrame.column_names.size() == capacity_so_far;
        add_or_replace(replace, lhsIndex, other_col[i]);
    }
    return *this;
}

DataFrame::DataFrameProxy& DataFrame::DataFrameProxy::operator=(
    vector<double>& other_col) {
    if (colNames.size() != 1)
        throw std::invalid_argument("different number of columns");
    int capacity_so_far = theDataFrame.column_names.size();
    int lhsIdx = find_or_add(colNames[0], theDataFrame.column_names);
    bool replace = theDataFrame.column_names.size() == capacity_so_far;
    add_or_replace(replace, lhsIdx, other_col);
    return *this;
}

DataFrame::DataFrameProxy& DataFrame::DataFrameProxy::operator=(
    vector<string>& other_col) {
    if (colNames.size() != 1)
        throw std::invalid_argument("different number of columns");
    int capacity_so_far = theDataFrame.column_names.size();
    int lhsIdx = find_or_add(colNames[0], theDataFrame.column_names);
    bool replace = theDataFrame.column_names.size() == capacity_so_far;
    add_or_replace(replace, lhsIdx, other_col);
    return *this;
}

std::ostream& operator<<(std::ostream& os,
                         const DataFrame::DataFrameProxy& df) {
    string output;
    //double num;
    for (string const& x : df.colNames) output += x + ' ';
    output += '\n';
    int num_rows = df.theDataFrame.size().first;
    for (int row = 0; row < num_rows; ++row) {
        for (string const& col : df.colNames) {
            int col_number = df.theDataFrame.column_names[col];
            string type = (*df.theDataFrame.columns[col_number]).get_type();
            if (type == "string") {
                string num =
                    (*df.theDataFrame.columns[col_number]).operator[]<string>(row);
                output += num + ' ';
            } else if (type == "double") {
                double num =
                    (*df.theDataFrame.columns[col_number]).operator[]<double>(row);
                output += std::to_string(num) + ' ';
            }
            else 
                throw std::invalid_argument("Does not work");
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
    std::pair<int, int> size = df.size();
    for (int row = 0; row < size.first; ++row) {
        for (auto const& col : df.column_names) {
            string type = (*df.columns[col.second]).get_type();
            if (type == "string") {
                string num = (*df.columns[col.second]).operator[]<string>(row);
                output += num + ' ';
            } else if (type == "double") {
                double num = (*df.columns[col.second]).operator[]<double>(row);
                output += std::to_string(num) + ' ';
            } else 
                throw std::invalid_argument("No matching type");
        }
        output += '\n';
    }
    os << output;
    return os;
}

DataFrame& DataFrame::operator+=(const DataFrame& rhs) {
    typedef std::numeric_limits<double> nan;
    for (auto& x : column_names) {
        DataFrameProxy df_tmp = DataFrameProxy(*this, x.first);
        try {
            int rhsIdx = rhs.column_names.at(x.first);
            if (columns[x.second]->get_type() == "double") {
                vector<double> tmp;
                add_elements<double>(tmp, rhs, x.second, rhsIdx);
                df_tmp.add_or_replace(true, x.second, tmp);
            } else if (columns[x.second]->get_type() == "string") {
                vector<string> tmp;
                add_elements<string>(tmp, rhs, x.second, rhsIdx);
                df_tmp.add_or_replace(true, x.second, tmp);
            }
        } catch (const std::out_of_range& e) {
            auto s = size().first;
            if (columns[x.second]->get_type() == "double") {
                vector<double> tmp = vector<double>(s, nan::quiet_NaN());
                df_tmp.add_or_replace(true, x.second, tmp);
            } else if (columns[x.second]->get_type() == "string") {
                vector<string> tmp = vector<string>(s);
                df_tmp.add_or_replace(true, x.second, tmp);
            }
        } catch (const std::invalid_argument& e) {
            ;
        }
    }
    //throw std::invalid_argument("Cannot get here"); //correct ordering?!?
}

DataFrame operator+(const DataFrame& lhs, const DataFrame& rhs) {
    DataFrame sum = lhs;
    sum += rhs;
    return sum;
}
