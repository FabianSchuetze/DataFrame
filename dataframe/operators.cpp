#include <iostream>
#include "dataframe.h"
#include "dataframeproxy.h"

using std::deque;
using std::make_pair;
using std::make_shared;
using std::pair;
using std::shared_ptr;
using std::string;
using std::transform;
using std::vector;

DataFrame::DataFrameProxy DataFrame::operator[](const vector<string>& names) {
    return DataFrameProxy(*this, names);
}

DataFrame::DataFrameProxy DataFrame::operator[](const string& name) {
    return operator[](vector<string>{name});
}

bool maybe_add(const string& name, std::map<string, int>& columns) {
    try {
        columns.at(name);
        return false;
    } catch (std::out_of_range& e) {
        columns[name] = columns.size();
    }
    return true;
}

void DataFrame::DataFrameProxy::replace_column(int idx,
                                               const shared_ptr<Column>& data) {
    if (theDataFrame.columns[idx].use_count() > 1)
        theDataFrame.columns.at(idx) = data;
    else
        theDataFrame.columns[idx] = data;
}

void DataFrame::DataFrameProxy::add_column(const shared_ptr<Column>& data) {
    theDataFrame.columns.push_back(data);
}

void DataFrame::DataFrameProxy::check_col_width(size_t check, string m) {
    if (colNames.size() != check) throw std::invalid_argument(m);
}

void DataFrame::DataFrameProxy::check_col_len(size_t check, string m) {
    if (theDataFrame.size().first != check) throw std::invalid_argument(m);
}

void DataFrame::DataFrameProxy::insert_column(const string& name,
                                              shared_ptr<Column>& inp) {
    bool add = maybe_add(name, theDataFrame.column_names);
    int lhsIdx = theDataFrame.find_column_position(name);
    add ? add_column(inp) : replace_column(lhsIdx, inp);
}

template <typename T>
void DataFrame::DataFrameProxy::insert_column(const string& name,
                                              const vector<T>& inp) {
    std::shared_ptr<Column> col = make_shared<Column>(Column(inp));
    insert_column(name, col);
}

void DataFrame::DataFrameProxy::insert_column(const vector<string>& rhsNames,
                                              const DataFrame& rhsDf) {
    for (size_t i = 0; i < rhsNames.size(); ++i) {
        SharedCol col = rhsDf.get_shared_copy(rhsNames[i]);
        check_col_len(col->size(), "column lenght of DataFrames differ");
        insert_column(colNames[i], col);
    }
}

DataFrame::DataFrameProxy& DataFrame::DataFrameProxy::operator=(
    const DataFrameProxy& rhs) {
    if (this != &rhs) {
        check_col_width(rhs.colNames.size(), string{"r & lhs col num differ"});
        try {
            insert_column(rhs.colNames, rhs.theDataFrame);
        } catch (...) {
            throw std::runtime_error("Assignment operator from DfProxy failed");
        }
    }
    return *this;
}

DataFrame::DataFrameProxy& DataFrame::DataFrameProxy::operator=(
    const DataFrame& rhs) {
    check_col_width(rhs.size().second, string{"rhs and lhs col num differ"});
    vector<string> rhs_names = rhs.get_column_names();
    try {
        insert_column(rhs.get_column_names(), rhs);
    } catch (...) {
        throw std::runtime_error("Assignment operator from Df failed");
    }
    return *this;
}

// NEEDS TO BE CHANGED BIG TIME!!!
DataFrame::DataFrameProxy& DataFrame::DataFrameProxy::operator=(
    const vector<vector<double>>& others) {
    check_col_width(others.size(),
                    string{"passed number of vectors != columns"});
    for (size_t i = 0; i < others.size(); ++i)
        check_col_len(others[i].size(), string{"len of vector num != colum"});
    for (size_t i = 0; i < colNames.size(); ++i)
        insert_column(colNames[i], others[i]);
    return *this;
}

template <typename T>
DataFrame::DataFrameProxy& DataFrame::DataFrameProxy::operator=(
    const vector<T>& other_col) {
    check_col_width(1, string{"must select one column"});
    check_col_len(other_col.size(), string{"len of vector num != colum"});
    insert_column(colNames[0], other_col);
    return *this;
}

template DataFrame::DataFrameProxy& DataFrame::DataFrameProxy::operator=(
    const vector<double>& other_col);
template DataFrame::DataFrameProxy& DataFrame::DataFrameProxy::operator=(
    const vector<string>& other_col);
template DataFrame::DataFrameProxy& DataFrame::DataFrameProxy::operator=(
    const vector<bool>& other_col);

DataFrame& DataFrame::operator+=(const DataFrame& rhs) {
    deque<pair<int, int>> indices = index.index_correspondence(rhs.index);
    duplicate_rows(indices);
    for (auto& x : column_names) {
        make_unique_if(x.first);
        try {
            columns[x.second]->plus(*rhs.get_shared_copy(x.first), indices);
        } catch (const std::out_of_range& e) {
            columns[x.second]->replace_nan();
        }
    }
    return *this;
}

void compare_both_indices(const Index& a, const Index& b, const char* caller) {
    std::pair<size_t, size_t> s_a = a.size();
    std::pair<size_t, size_t> s_b = b.size();
    if ((s_a.first != 0) & (s_b.first != 0) & (s_a.second != s_b.second)) {
        std::string m("Indices have different depth, in:\n");
        throw std::invalid_argument(m + caller);
    }
}

DataFrame operator+(const DataFrame& lhs, const DataFrame& rhs) {
    compare_both_indices(lhs.index, rhs.index, __PRETTY_FUNCTION__);
    DataFrame sum = deep_copy(lhs);
    sum.append_missing_cols(rhs);
    sum.append_missing_rows(rhs);
    sum += rhs;
    sum.assert_same_column_length(__PRETTY_FUNCTION__);
    return sum;
}

DataFrame operator+(const DataFrame::DataFrameProxy& lhs,
                    const DataFrame::DataFrameProxy& rhs) {
    return DataFrame(lhs) + DataFrame(rhs);
}

DataFrame operator+(const DataFrame& lhs,
                    const DataFrame::DataFrameProxy& rhs) {
    return lhs + DataFrame(rhs);
}
