#include "dataframe.h"
#include <iostream>
#include <ostream>
#include <stdexcept>
#include "dataframeproxy.h"
using std::make_pair;
using std::make_shared;
using std::pair;
using std::shared_ptr;
using std::string;
using std::vector;

vector<int> get_index_positions(const vector<pair<string, int>> vec) {
    vector<int> res;
    for (auto const& x : vec) res.push_back(x.second);
    return res;
}

template <typename T>
vector<string> get_names(T& cont) {
    vector<string> res;
    for (auto const& p : cont) res.push_back(p.first);
    return res;
}

vector<string> DataFrame::get_index_names() { return get_names(index_names); }
vector<string> DataFrame::get_index_names() const { return get_names(index_names); }

vector<string> DataFrame::get_column_names() {return get_names(column_names);}
vector<string> DataFrame::get_column_names() const {return get_names(column_names);}

DataFrame::DataFrame() : columns(), index_names(), column_names(){};

std::shared_ptr<Column> DataFrame::get_unique(const std::string& s) {
    return static_cast<const DataFrame&>(*this).get_unique(s);  // Item 3
}

std::shared_ptr<Column> DataFrame::get_unique(const std::string& s) const {
    try {
        int idx = column_names.at(s);
        return make_shared<Column>(*columns.at(idx));
    } catch (const std::out_of_range& e) {
        string msg = "No column with name " + s;
        throw std::out_of_range(msg);
    }
}

std::shared_ptr<Column> DataFrame::get_shared_copy(const std::string& s) {
    return static_cast<const DataFrame&>(*this).get_shared_copy(s);  // Item 3
}

std::shared_ptr<Column> DataFrame::get_shared_copy(const std::string& s) const
{
    try {
        int idx = column_names.at(s);
        return columns[idx];
    } catch (const std::out_of_range& e) {
        string msg = "No column with name " + s;
        throw std::out_of_range(msg);
    }
}

shared_ptr<Column> DataFrame::get_unique(const string& s,
                                         const vector<int>& v) const {
    int i = column_names.at(s);
    Column new_col = Column(*columns[i], v);
    return make_shared<Column>(new_col);
}

const int DataFrame::find_index_position(const std::string& s) const {
    for (const auto& x : index_names) {
        if (x.first == s) return x.second;
    }
    return -1;
}

const int DataFrame::find_index_position(const std::string& s) {
    return static_cast<const DataFrame&>(*this).find_index_position( s);
}

DataFrame deep_copy(const DataFrame& lhs) {
    DataFrame new_df = DataFrame();
    vector<int> old_positions = get_index_positions(lhs.index_names);
    int i = 0, j = 0;
    for (auto const& x : lhs.column_names) {
        new_df.column_names[x.first] = i++;
        new_df.columns.push_back(lhs.get_unique(x.first, old_positions));
    }
    for (auto const& x : lhs.index_names)
        new_df.index_names.push_back(make_pair(x.first, j++));
    return new_df;
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
    for (const string& name : df.colNames) {
        int pos = df.theDataFrame.column_names[name];
        columns.push_back(df.theDataFrame.columns[pos]);
        column_names[name] = i++;
    }
    for (const string& name : df.idxNames) {
        int pos = df.theDataFrame.find_index_position(name);
        index_names.push_back(make_pair(name, pos));
    }
}

const std::pair<int, int> DataFrame::size() const {
    return make_pair(index_names.size(), columns.size());
}

const int DataFrame::use_count(string col) {
    return columns[column_names[col]].use_count();
}

vector<pair<int, int>> correspondence_position(const DataFrame& lhs,
                                               const DataFrame& other) {
    vector<pair<int, int>> res;
    int i = 0;
    for (auto const& x : lhs.index_names) {
        int other_pos = other.find_index_position(x.first);
        res.push_back(make_pair(i++, other_pos));
    }
    return res;
}

void DataFrame::append_nan_rows() {
    for (auto& x : column_names) columns[x.second]->push_back_nan();
}

void DataFrame::append_index(const string& s) {
    index_names.push_back(make_pair(s, index_names.size()));
}
