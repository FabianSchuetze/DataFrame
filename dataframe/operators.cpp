#include <iostream>
#include "dataframe.h"
#include "dataframeproxy.h"

using std::make_pair;
using std::make_shared;
using std::pair;
using std::shared_ptr;
using std::string;
using std::transform;
using std::vector;

DataFrame::DataFrameProxy DataFrame::operator[](const vector<string>& names) {
    return DataFrameProxy(*this, get_index_names(), names);
}

DataFrame::DataFrameProxy DataFrame::operator[](const string& name) {
    return operator[](vector<string>{name});
}

DataFrame::DataFrameProxy DataFrame::loc(const string& s) {
    return DataFrameProxy(*this, vector<string>{s}, get_column_names());
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

DataFrame& DataFrame::operator=(const DataFrame& rhs) {
    if (this != &rhs) {
        columns = rhs.columns;
        index_names = rhs.index_names;
        index_positions = rhs.index_positions;
        column_names = rhs.column_names;
    }
    return *this;
}

DataFrame::DataFrameProxy& DataFrame::DataFrameProxy::operator=(
    const DataFrameProxy& rhs) {
    if (this != &rhs) {
        check_col_width(rhs.colNames.size(), string{"r & lhs col num differ"});
        try{
            insert_column(rhs.colNames, rhs.theDataFrame);
        } catch(...) {
            throw std::runtime_error("Assignment operator from DfProxy failed");
        }
    }
    return *this;
}

DataFrame::DataFrameProxy& DataFrame::DataFrameProxy::operator=(
        const DataFrame& rhs) {
    check_col_width(rhs.size().second, string{"rhs and lhs col num differ"});
    vector<string> rhs_names = rhs.get_column_names();
    try{
        insert_column(rhs.get_column_names(), rhs);
    } catch(...) {
        throw std::runtime_error("Assignment operator from Df failed");
    }
    return *this;
}

DataFrame::DataFrameProxy& DataFrame::DataFrameProxy::operator=(
    const vector<vector<double>>& others) {
    check_col_width(others.size(), string{"passed number of vectors != columns"});
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
    std::deque<pair<int, int>> indices = correspondence_position(*this, rhs);
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

void append_missing_rows(DataFrame& lhs, const DataFrame& rhs) {
    std::deque<pair<int, int>> index_pairs = correspondence_position(rhs, lhs);
    for (auto const& index_pair : index_pairs) {
        if (index_pair.second == -1) {
            lhs.append_nan_rows();
            lhs.append_index(rhs.index_positions[index_pair.first]);
        }
    }
}

void append_missing_cols(DataFrame& lhs, const DataFrame& rhs) {
    for (auto const& x : rhs.column_names) {
        bool add = maybe_add(x.first, lhs.column_names);
        int lhsPos = lhs.find_column_position(x.first);
        if (add) {
            lhs.columns.push_back(rhs.get_unique(x.first));
            lhs.columns.at(lhsPos)->replace_nan();
        }
    }
}

DataFrame operator+(const DataFrame& lhs, const DataFrame& rhs) {
    DataFrame sum = deep_copy(lhs);
    append_missing_cols(sum, rhs);
    append_missing_rows(sum, rhs);
    return sum += rhs;
}

DataFrame operator+(const DataFrame::DataFrameProxy& lhs,
                    const DataFrame::DataFrameProxy& rhs) {
    return DataFrame(lhs) + DataFrame(rhs);
}

DataFrame operator+(const DataFrame& lhs,
                    const DataFrame::DataFrameProxy& rhs) {
    return lhs + DataFrame(rhs);
}

template <typename T>
DataFrame& DataFrame::operator+=(const T& t) {
    for (auto& x : column_names) {
        Column new_col = *columns[x.second] + t;
        columns[x.second] = std::make_shared<Column>(new_col);
    }
    return *this;
}
template DataFrame& DataFrame::operator+=(const double&);
template DataFrame& DataFrame::operator+=(const int&);
template DataFrame& DataFrame::operator+=(const string&);
template DataFrame& DataFrame::operator+=(const bool&);
