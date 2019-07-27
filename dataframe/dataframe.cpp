#include "dataframe.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <numeric>
#include <ostream>
#include <set>
#include <stdexcept>
#include <unordered_set>
#include "ConstColumnIterator.h"
#include "dataframeproxy.h"
//#include "GroupbyFunctions.h"
//#include "grouper.h"
class ColumnIterator;
using std::deque;
using std::make_pair;
using std::make_shared;
using std::pair;
using std::shared_ptr;
using std::string;
using std::vector;

void equal_width(size_t num_names, size_t num_cols, const char* fun) {
    if (!(num_names == num_cols)) {
        std::string s("Number of columns: " + std::to_string(num_cols) +
                      " but " + std::to_string(num_names) +
                      " column names. In:\n");
        throw std::invalid_argument(s + fun);
    }
}

DataFrame::DataFrame(const Index& idx) {
    index = idx;
}

bool maybe_add(const string&, std::map<string, int>&);

void missing_col_error(const char* what, string s) {
    std::string msg = " Column " + s + " not found";
    throw std::out_of_range(what + msg);
}

template <typename t>
vector<string> get_names(t& cont) {
    vector<string> res;
    std::transform(cont.begin(), cont.end(), std::back_inserter(res),
                   [](const pair<string, int>& ele) { return ele.first; });
    return res;
}

vector<string> DataFrame::get_column_names() { 
    return static_cast<const DataFrame&>(*this).get_column_names();
}

template <typename T>
vector<string> get_names(const T& inp) {
    vector<string> o(inp.size());
    auto b = inp.begin();
    auto e = inp.end();
    std::transform(b, e, o.begin(), [](const auto& ele) { return ele.first; });
    return o;
}
vector<string> DataFrame::get_column_names() const {
    return get_names(column_names);
}

template <typename T>
vector<string> DataFrame::get_column_names() {
    string target;
    if (std::is_same<T, double>::value) target = "double";
    if (std::is_same<T, string>::value) target = "string";
    auto fun = [&](const pair<string, int>& x) {
        return columns[x.second]->type_name() == target ? true : false;
    };
    std::vector<std::pair<string, int>> res;
    std::copy_if(column_names.begin(), column_names.end(),
                 std::back_inserter(res), fun);
    return get_names(res);
}

template vector<string> DataFrame::get_column_names<double>();
template vector<string> DataFrame::get_column_names<string>();

DataFrame::DataFrame()
    : columns(), column_names() {Index();}

std::shared_ptr<Column> DataFrame::get_unique(const std::string& s) {
    return static_cast<const DataFrame&>(*this).get_unique(s);  // Item 3
}

std::shared_ptr<Column> DataFrame::get_unique(const std::string& s) const {
    try {
        int idx = column_names.at(s);
        return make_shared<Column>(*columns.at(idx));
    } catch (const std::out_of_range& e) {
        throw std::out_of_range("No column with name" + s);
    }
}

shared_ptr<Column> DataFrame::get_unique(const string& s,
                                         const deque<int>& v) const {
    Column new_col = Column(*columns[find_column_position(s)], v);
    return make_shared<Column>(new_col);
}

std::shared_ptr<Column> DataFrame::get_shared_copy(const std::string& s) {
    return static_cast<const DataFrame&>(*this).get_shared_copy(s);
}

std::shared_ptr<Column> DataFrame::get_shared_copy(const std::string& s) const {
    try {
        int idx = column_names.at(s);
        return columns[idx];
    } catch (const std::out_of_range& e) {
        string m = "No col with name " + s + " in\n" + __PRETTY_FUNCTION__;
        throw std::out_of_range(m);
    }
}

void DataFrame::make_contigious() {
    DataFrame new_df = deep_copy(*this);
    *this = new_df;
}

DataFrame deep_copy(const DataFrame& lhs) {
    DataFrame new_df = DataFrame();
    deque<int> old_positions = lhs.index.find_index_position();
    for (auto const& x : lhs.column_names) {
        new_df.append_column(x.first, lhs.get_unique(x.first, old_positions));
    }
    new_df.index.append_index(lhs.index.index_positions);
    return new_df;
}

void DataFrame::append_column(const string& name, const SharedCol& col) {
    columns.push_back(col);
    column_names[name] = column_names.size();
}

DataFrame::DataFrame(const DataFrame::DataFrameProxy& df)
    : columns(), index(df.proxy_index), column_names() {
    try {
        for (const string& name : df.colNames)
            append_column(name, df.theDataFrame.get_shared_copy(name));
    } catch (std::out_of_range& e) {
        string m = string("In\n") + __PRETTY_FUNCTION__ + ":\n" + e.what();
        throw std::out_of_range(m);
    }
}

std::pair<size_t, size_t> DataFrame::size() const {
    return make_pair(index.size().first, columns.size());
}

int DataFrame::use_count(const string& name) {
    int pos = find_column_position(name);
    return columns[pos].use_count();
}

void DataFrame::copy_row(int pos) {
    for (auto const x : column_names)
        columns[x.second]->copy_row(pos);
}

void DataFrame::duplicate_rows(int pos) {
    index.append_index(index.index_positions[pos]);
    copy_row(pos);
}

void DataFrame::duplicate_rows(deque<pair<int, int>>& indices) {
    std::set<int> s;
    for (auto& pair : indices) {
        if (s.count(pair.first)) {
            duplicate_rows(pair.first);
            //pair.first = index.index_positions.size() - 1;
        } else
            s.insert(pair.first);
    }
}

void DataFrame::append_nan_rows() {
    for (auto& x : column_names) {
        columns[x.second]->push_back_nan();
    }
    assert_same_column_length(__PRETTY_FUNCTION__);
}

int DataFrame::find_column_position(const std::string& s) {
    return static_cast<const DataFrame&>(*this).find_column_position(
        s);  // Item 3
}

int DataFrame::find_column_position(const std::string& s) const {
    try {
        return column_names.at(s);
    } catch (std::out_of_range& e) {
        throw std::out_of_range("Column " + s + " not found");
    }
}

vector<int> DataFrame::contains_null() {
    vector<int> inp(columns[0]->size(), 0);
    for (const auto& cols : column_names) {
        columns[cols.second]->is_null(inp);
    }
    return inp;
}

void DataFrame::make_unique(const std::string& s) {
    columns.at(find_column_position(s)) = get_unique(s);
}

void DataFrame::make_unique(const std::vector<string>& vec) {
    for (const string& s : vec) make_unique(s);
}

void DataFrame::make_unique_if(const std::string& s) {
    if (this->use_count(s) > 1) {
        std::cout << "copy-on-write\n";
        make_unique(s);
    }
}

void DataFrame::make_unique_if(const vector<string>& c) {
    for (const string& s : c) make_unique_if(s);
}

void DataFrame::drop_row(std::deque<Index::ele> e) {
    vector<deque<Index::ele>> tmp{e};
    return drop_row(tmp);
}

void DataFrame::drop_row(vector<deque<Index::ele>> vec) {
    std::sort(vec.begin(), vec.end());
    auto fun = [&](const auto& val) {
        return !std::binary_search(vec.begin(), vec.end(), val);
    };
    auto it = std::stable_partition(index.index_positions.begin(),
                                    index.index_positions.end(), fun);
    index.index_positions.erase(it, index.index_positions.end());
    for (auto& v : vec) index.index_names.erase(v);
}

void DataFrame::dropna() {
    vector<int> count = contains_null();
    auto fun = [&](const auto& e) -> bool {
        int pos = index.find_index_position(e).front();
        index.index_names[e].pop_front();
        if (count[pos] > 0) return true;
        index.index_names[e].push_back(pos);
        return false;
    };
    auto b = index.index_positions.begin();
    auto e = index.index_positions.end();
    index.index_positions.erase(std::remove_if(b, e, fun), e);
}


void DataFrame::drop_column(const string& s) {
    // WHAT HAPPENS IF THERE IS ONLY ONE SHARED POINTER OF IT? DOES THE THINK
    // DROP OUT OF THE VECTOR?
    columns[find_column_position(s)].~shared_ptr();  // reduce use_count()
    column_names.erase(s);                           // delete reference to it;
}

void DataFrame::sort_by_index() {
    auto b = index.index_positions.begin();
    auto e = index.index_positions.end();
    std::sort(b, e, [](auto& a, auto& b) { return a < b; });
}

bool DataFrame::is_contigious() {
    // HERE THE FUNCTION WOULD STILL RETURN THE SAME INTERFACE
    deque<int> existing_order = index.find_index_position();
    for (size_t i = 1; i < existing_order.size(); ++i)
        if ((existing_order[i] - existing_order[i - 1]) != 1) return false;
    return true;
}
void DataFrame::convert_bool_to_double(const std::string& s) {
    int pos = find_column_position(s);
    columns[pos]->convert_bool_to_double();
}

void DataFrame::assert_same_column_length(const char* pass) {
    for (auto const x : column_names) {
        if (columns[0]->size() != columns[x.second]->size()) {
            string msg("Column " + x.first + " has different len, in:\n");
            throw std::runtime_error(msg + pass);
        }
    }
}

DataFrame::DataFrame(std::ifstream& file)
    : columns(), column_names() {
    vector<string> colNames = create_column_names(file);
    initialize_column(file, colNames);
    insert_data(file, colNames);
    assert_same_column_length(__PRETTY_FUNCTION__);
}

void DataFrame::append_missing_rows(const DataFrame& rhs) {
    deque<pair<int, int>> pairs = rhs.index.index_correspondence(this->index);
    for (auto const& pair : pairs) {
        if (pair.second == -1) {
            append_nan_rows();
            index.append_index(rhs.index.index_positions[pair.first]);
        }
    }
}

std::shared_ptr<Column> empty_Column(size_t sz, string type) {
    Column col;
    if (type == "double") {
        typedef std::numeric_limits<double> nan;
        col = Column(vector<double>(sz, nan::quiet_NaN()));
    } else if (type == "string")
        col = Column(vector<string>(sz, "NA"));
    else {
        string m = "Cannot create empty Column";
        throw std::runtime_error(m + __PRETTY_FUNCTION__);
    }
    return std::make_shared<Column>(col);
}

void DataFrame::append_missing_cols(const DataFrame& rhs) {
    for (auto const& x : rhs.column_names) {
        if (maybe_add(x.first, column_names)) {
            size_t len = size().first;
            string type = rhs.columns[x.second]->type_name();
            columns.push_back(empty_Column(len, type));
        }
    }
}
DataFrame::DataFrameProxy DataFrame::loc(const std::deque<Index::ele>& idx,
                                         const std::string& s) {
    return DataFrameProxy(*this, idx, s);
}
DataFrame::DataFrameProxy DataFrame::loc(
        const std::vector<std::deque<Index::ele>>& idx,
        const std::vector<std::string>& vec) {
    return DataFrameProxy(*this, idx, vec);
}
