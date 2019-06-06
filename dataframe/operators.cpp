#include <iostream>
#include "dataframe.h"
#include "dataframeproxy.h"

using std::make_pair;
using std::make_shared;
using std::shared_ptr;
using std::pair;
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

DataFrame& DataFrame::operator=(const DataFrame& rhs) {
    if (this != &rhs) {
        columns = rhs.columns;
        index_names = rhs.index_names;
        column_names = rhs.column_names;
    }
    return *this;
}

int find_or_add(const string& name, std::map<string, int>& columns) {
    try {
        columns.at(name);
    } catch (const std::out_of_range& e) {
        columns[name] = columns.size();
    }
    return columns.at(name);
}

void DataFrame::make_unique_if(const std::string& s) {
    if (this->use_count(s) > 1) {
        std::cout << "copy-on-write\n";
        std::shared_ptr<Column> data = get_unique(s);
        columns.at(column_names[s]) = data;
    }
}

void DataFrame::DataFrameProxy::replace_column(int idx, 
        const shared_ptr<Column>&data) {
    if (theDataFrame.columns[idx].use_count() > 1)
        theDataFrame.columns.at(idx) = data;
    else
        theDataFrame.columns[idx] = data;
}

void DataFrame::DataFrameProxy::add_column(const shared_ptr<Column>& data)
{
    theDataFrame.columns.push_back(data);
}

void DataFrame::DataFrameProxy::check_size(size_t check, string m) {
    if (colNames.size() != check)
        throw std::invalid_argument(m);
}

void DataFrame::DataFrameProxy::insert_column(const string& name,
                                              shared_ptr<Column>& inp) {
    int current_cap = theDataFrame.column_names.size();
    int lhsIdx = find_or_add(name, theDataFrame.column_names);
    bool replace = theDataFrame.column_names.size() == current_cap;
    replace ? replace_column(lhsIdx, inp) : add_column(inp);
}

template <typename T>
void DataFrame::DataFrameProxy::insert_column(const string& name,
                                              const vector<T>& inp) {
    std::shared_ptr<Column> col = make_shared<Column>(Column(inp));
    insert_column(name, col);
}

DataFrame::DataFrameProxy& DataFrame::DataFrameProxy::operator=(
    const DataFrameProxy& rhs) {
    check_size(rhs.colNames.size(), string{"rhs and lhs column number differ"});
    for (size_t i = 0; i < colNames.size(); ++i) {
        string rhsName = rhs.colNames[i];
        shared_ptr<Column> col = rhs.theDataFrame.get_shared_copy(rhsName);
        insert_column(colNames[i], col);
    }
    return *this;
}

DataFrame::DataFrameProxy& DataFrame::DataFrameProxy::operator=(
    const vector<vector<double>>& others) {
    check_size(others.size(), string{"passed number of vectors != columns"});
    for (size_t i = 0; i < colNames.size(); ++i)
        insert_column(colNames[i], others[i]);
    return *this;
}

template <typename T>
DataFrame::DataFrameProxy& DataFrame::DataFrameProxy::operator=(
    const vector<T>& other_col) {
    check_size(1, string{"must select one column"}); 
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
    string output = " ";
    for (string const& x : df.colNames) output += x + ' ';
    output += '\n';
    for (string const& row_name : df.idxNames) {
        int row = df.theDataFrame.find_index_position(row_name);
        if (row == -1) throw std::invalid_argument("Element not found");
        output += row_name + " ";
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
    string output = " ";
    for (auto const& x : df.column_names) output += x.first + ' ';
    output += '\n';
    for (auto const& row : df.index_names) {
        output += row.first + " ";
        for (auto const& col : df.column_names)
            append_string(*df.columns[col.second], output, row.second);
        output += '\n';
    }
    os << output;
    return os;
}

DataFrame& DataFrame::operator+=(const DataFrame& rhs) {
    vector<pair<int, int>> indices = correspondence_position(*this, rhs);
    for (auto& x : column_names) {
        make_unique_if(x.first);
        try {
            // IS THERE A COPY INVOLVED? WHY DON'T PASS A SHARED POINTER?!?
            columns[x.second]->plus(*rhs.get_shared_copy(x.first), indices);
        } catch (const std::out_of_range& e) {
            columns[x.second]->replace_nan();
        }
    }
    return *this;
}

void append_missing_rows(DataFrame& lhs, const DataFrame& rhs) {
    vector<pair<int, int>> index_pairs = correspondence_position(rhs, lhs);
    for (auto const& index_pair : index_pairs) {
        if (index_pair.second == -1) {
            lhs.append_nan_rows();
            lhs.append_index(rhs.index_names[index_pair.first].first);
        }
    }
}

void append_missing_cols(DataFrame& lhs, const DataFrame& rhs) {
    for (auto const& x : rhs.column_names) {
        size_t capacity_so_far = lhs.column_names.size();
        int lhsPos = find_or_add(x.first, lhs.column_names);
        if (capacity_so_far < lhs.column_names.size()) {
            std::shared_ptr<Column> data = rhs.get_unique(x.first);
            lhs.columns.push_back(data);
            lhs.columns.at(lhsPos)->replace_nan();
        }
    }
}

DataFrame operator+(const DataFrame& lhs, const DataFrame& rhs) {
    DataFrame sum = deep_copy(lhs);
    append_missing_cols(sum, rhs);
    append_missing_rows(sum, rhs);
    return sum+= rhs;
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
