#include <iostream>
#include "dataframe.h"
#include "dataframeproxy.h"

using std::make_pair;
using std::make_shared;
using std::pair;
using std::string;
using std::transform;
using std::vector;

DataFrame::DataFrameProxy DataFrame::operator[](const vector<string>& names) {
    vector<string> idx = get_index_names();
    return DataFrameProxy(*this, idx, names);
}

DataFrame::DataFrameProxy DataFrame::operator[](const string& name) {
    return operator[](vector<string>{name});
}

DataFrame::DataFrameProxy DataFrame::loc(const string& s) {
    vector<string> idx = {s};
    vector<string> cols;
    for (auto const& x : column_names) cols.push_back(x.first);
    return DataFrameProxy(*this, idx, cols);
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

// DO I REALLY NEED THIS FUNCTION?!?
//template <typename T>
//void DataFrame::DataFrameProxy::add_or_replace(bool replace, int idx,
                                               //const vector<T>& data) {
    //if (replace) {
        //if (theDataFrame.columns[idx].use_count() > 1) {
            //std::cout << "copy-on-write\n";
            //theDataFrame.columns.at(idx) = make_shared<Column>(data);
        //} else {
            ////theDataFrame.columns[idx].reset(make_shared<Column>(data));
            //*theDataFrame.columns[idx] = data;
        //}
    //} else
        //theDataFrame.columns.push_back(make_shared<Column>(data));
//}

void DataFrame::make_unique_if(const std::string& s) {
    if (this->use_count(s) > 1) {
        std::cout << "copy-on-write\n";
        std::shared_ptr<Column> data = get_unique(s);
        columns.at(column_names[s]) = data;
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

void DataFrame::DataFrameProxy::insert_column(const string& name,
                                              std::shared_ptr<Column>& inp) {
    std::cout << "calling this function2\n";
    size_t current_cap = theDataFrame.size().second;
    int lhsIndex = find_or_add(name, theDataFrame.column_names);
    bool replace = theDataFrame.size().second == current_cap;
    add_or_replace(replace , lhsIndex, inp);
}

template <typename T>
void DataFrame::DataFrameProxy::insert_column(const string& name,
                                              const vector<T>& inp) {
    std::cout << "calling this function\n";
    Column col = Column(inp);
    std::shared_ptr<Column> a = make_shared<Column>(col);
    insert_column(name, a);
}


DataFrame::DataFrameProxy& DataFrame::DataFrameProxy::operator=(
    const DataFrameProxy& rhs) {
    check_column_size(rhs.colNames.size());
    for (size_t i = 0; i < colNames.size(); ++i) {
        int rhsIndex = rhs.theDataFrame.column_names.at(rhs.colNames[i]);
        insert_column(colNames[i], rhs.theDataFrame.columns[rhsIndex]);
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
            int rhsIdx = rhs.column_names.at(x.first);
            columns[x.second]->add_other_column(*rhs.columns[rhsIdx], indices);
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
            for (auto& x : lhs.column_names)
                lhs.columns[x.second]->push_back_nan();
            lhs.index_names.push_back(
                make_pair(rhs.index_names[index_pair.first].first,
                          lhs.index_names.size()));
        }
    }
}

void append_missing_cols(DataFrame& lhs, const DataFrame& rhs) {
    for (auto const& x : rhs.column_names) {
        size_t capacity_so_far = lhs.column_names.size();
        int lhsPos = find_or_add(x.first, lhs.column_names);
        if (capacity_so_far < lhs.column_names.size()) {
            std::shared_ptr<Column> data =
                make_shared<Column>(*rhs.columns.at(x.second));
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
