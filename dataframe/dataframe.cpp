#include "dataframe.h"
#include <algorithm>
#include <iostream>
#include <numeric>
#include <chrono>
#include <ostream>
#include <stdexcept>
#include <sstream>
#include "ConstColumnIterator.h"
#include "dataframeproxy.h"
class ColumnIterator;
using std::make_pair;
using std::make_shared;
using std::pair;
using std::shared_ptr;
using std::string;
using std::vector;

void DataFrame::missing_col_error(const char* what, const string& s) {
    std::string msg = " Column " + s + " not found";
    throw std::out_of_range(what + msg);
}
vector<int> DataFrame::get_index_positions(const vector<string>& inp) const {
    vector<int> res(inp.size(), 0);
    std::transform(inp.begin(), inp.end(), res.begin(),
                   [this](const string& s) { return find_index_position(s); });
    return res;
}

vector<int> DataFrame::get_index_positions() const {
    vector<int> res;
    std::transform(index_names.begin(), index_names.end(), back_inserter(res),
                   [](const pair<string, int>& ele) { return ele.second; });
    return res;
}

template <typename T>
vector<string> get_names(T& cont) {
    vector<string> res;
    std::transform(cont.begin(), cont.end(), std::back_inserter(res),
                   [](const pair<string, int>& ele) { return ele.first; });
    return res;
}

vector<string> DataFrame::get_index_names() { return get_names(index_names); }
vector<string> DataFrame::get_index_names() const {
    return get_names(index_names);
}

vector<string> DataFrame::get_column_names() { return get_names(column_names); }
vector<string> DataFrame::get_column_names() const {
    return get_names(column_names);
}

DataFrame::DataFrame() : columns(), index_names(), column_names() {}

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

shared_ptr<Column> DataFrame::get_unique(const string& s,
                                         const vector<int>& v) const {
    Column new_col = Column(*columns[get_column_position(s)], v);
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
        string msg = "No column with name " + s;
        throw std::out_of_range(msg);
    }
}

int DataFrame::find_index_position(const string& s) const {
    for (const auto& x : index_names) {
        if (x.first == s) return x.second;
    }
    return -1;
}

template <typename T>
int get_pair(const T& cont, const pair<string, int>& ele) {
    int pos = -1;
    for (const auto& x : cont) {
        if (x.first == ele.first && x.second == ele.second) return pos;
        pos++;
    }
    return pos;
}

int DataFrame::find_index_pair(const pair<string, int>& ele) {
    int pos = 0;
    for (const auto& x : index_names) {
        if (x.first == ele.first && x.second == ele.second) return pos;
        pos++;
    }
    throw std::out_of_range("Could not find the index argument");
}

int DataFrame::find_index_position(const std::string& s) {
    return static_cast<const DataFrame&>(*this).find_index_position(s);
}

void DataFrame::make_contigious() {
    DataFrame new_df = deep_copy(*this);
    *this = new_df;
}

DataFrame deep_copy(const DataFrame& lhs) {
    DataFrame new_df = DataFrame();
    vector<int> old_positions = lhs.get_index_positions();
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
template DataFrame::DataFrame(const vector<string>&, const vector<string>&,
                              const vector<vector<bool>>&);

DataFrame::DataFrame(const DataFrame::DataFrameProxy& df)
    : columns(), index_names(), column_names() {
    int i = 0;
    for (const string& name : df.colNames) {
        columns.push_back(df.theDataFrame.get_shared_copy(name));
        column_names[name] = i++;
    }
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    std::cout << "first\n";
    vector<int> res = df.theDataFrame.get_index_positions(df.idxNames);
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>( t2 - t1 ).count();
    std::cout << "completed in: " + std::to_string(duration) + "\n";
    std::transform(df.idxNames.begin(), df.idxNames.end(),
            std::back_inserter(index_names),
            [this](const string& s){return make_pair(s, find_index_position(s));});
    //for (const string& name : df.idxNames) {
        //int pos = df.theDataFrame.find_index_position(name);
        //if (pos == -1)
            //throw std::runtime_error("Could not find the index: " + name);
        //index_names.push_back(make_pair(name, pos));
    //}
}

std::pair<size_t, size_t> DataFrame::size() const {
    return make_pair(index_names.size(), columns.size());
}

int DataFrame::use_count(const string& name) {
    int pos = get_column_position(name);
    return columns[pos].use_count();
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

int DataFrame::get_column_position(const std::string& s) {
    return static_cast<const DataFrame&>(*this).get_column_position(
        s);  // Item 3
}

int DataFrame::get_column_position(const std::string& s) const {
    try {
        return column_names.at(s);
    } catch (std::out_of_range& e) {
        throw std::out_of_range("Column " + s + " not found");
    }
}

bool DataFrame::contains_null(const string& s) {
    int pos = find_index_position(s);
    for (auto const& cols : column_names) {
        if (columns[cols.second]->is_null(pos)) return true;
    }
    return false;
}

void DataFrame::make_unique(const std::string& s) {
    columns.at(get_column_position(s)) = get_unique(s);
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

void DataFrame::drop_row(const string& s) {
    int pos = find_index_pair(make_pair(s, find_index_position(s)));
    index_names.erase(index_names.begin() + pos);
}

// I THINK THIS IS NOT EXCEPTION SAFE!!!
void DataFrame::dropna() {
    vector<pair<string, int>> index_copy = index_names;
    for (auto index_pair : index_copy) {
        if (contains_null(index_pair.first)) {
            make_unique_if(get_column_names());
            drop_row(index_pair.first);
        }
    }
}

template <typename T>
vector<string> DataFrame::get_column_names() {
    string target;
    if (std::is_same<T, double>::value) target = "double";
    if (std::is_same<T, string>::value) target = "string";
    auto fun = [&](const pair<string, int>& x) {
        return columns[x.second]->type_name() == target ? true : false;
    };
    vector<pair<string, int>> res;
    std::copy_if(column_names.begin(), column_names.end(),
                 std::back_inserter(res), fun);
    return get_names(res);
}

template vector<string> DataFrame::get_column_names<double>();
template vector<string> DataFrame::get_column_names<string>();

void DataFrame::drop_column(const string& s) {
    columns[get_column_position(s)].~shared_ptr();  // reduce use_count()
    column_names.erase(s);                          // delete reference to it;
}

void DataFrame::sort_by_index() {
    std::sort(index_names.begin(), index_names.end(),
              [](auto& a, auto& b) { return a.first < b.first; });
}

void DataFrame::sort_by_column(const std::string& s) {
    string type = columns[get_column_position(s)]->type_name();
    if (type== "double")
        sort_by_column_template<double>(s);
    else if (type== "string")
        sort_by_column_template<string>(s);
    else if (type == "bool") {
        string msg = "Column: " + s + " is type bool, no sort implemented";
        throw std::logic_error(msg);
    }
}

template <typename T>
void DataFrame::sort_by_column_template(const string& s) {
    vector<pair<string, int>> new_index(index_names.size());
    vector<int> argsort = permutation_index<T>(s);
    for (size_t i = 0; i < argsort.size(); ++i) {
        pair<string, int> tmp = index_names[argsort[i]];
        new_index[i] = tmp;
    }
    index_names = new_index;
}

template void DataFrame::sort_by_column_template<double>(const std::string&);
template void DataFrame::sort_by_column_template<std::string>(const std::string&);

bool DataFrame::is_contigious() {
    vector<int> existing_order = get_index_positions();
    for (size_t i = 1; i < existing_order.size(); ++i)
        if ((existing_order[i] - existing_order[i - 1]) != 1) return false;
    return true;
}
void DataFrame::convert_bool_to_double(const std::string& s) {
    int pos = get_column_position(s);
    columns[pos]->convert_bool_to_double();
}

vector<string>  DataFrame::create_column_names(std::ifstream& file) {
    std::string line, name;
    std::getline(file, line);
    size_t i = 0;
    vector<string>  cols;
    std::istringstream names(line);
    std::getline(names, name, ','); // pass the index column
    while (std::getline(names, name, ',')) {
        cols.push_back(name);
        try {
            column_names.at(name);
            throw std::invalid_argument("Duplicate column name " + name);
        } catch(std::out_of_range& e) {
            column_names[name] = i++;
        }
    }
    return cols;
}

template <typename T>
void DataFrame::initialize_column(const string& s) {
    Column col = Column(vector<T>());
    columns[get_column_position(s)] = make_shared<Column>(col);
}

void DataFrame::initialize_column(std::ifstream& file, const vector<string>&cols) {
    columns.resize(cols.size());
    std::string line, name;
    std::getline(file, line);
    std::istringstream names(line);
    std::getline(names, name, ','); // pass the index column
    size_t i = 0;
    while (std::getline(names, name, ',')) {
        string colName = cols[i++];
        if (name == "double")
            initialize_column<double>(colName);
        else if (name == "string")
            initialize_column<string>(colName);
        else if (name == "bool")
            initialize_column<bool>(colName);
        else 
            throw std::invalid_argument("Input type: " + name + " incompatible");
    }
}

void DataFrame::insert_data(std::ifstream& file, const vector<string>& cols) {
    std::string line, name;
    size_t line_number = 0;
    while (std::getline(file, line)) {
        size_t i = 0;
        std::istringstream names(line);
        std::getline(names, name, ',');
        index_names.push_back(make_pair(name, line_number++));
        while (std::getline(names, name, ',')) {
            int pos = get_column_position(cols[i++]);
            columns[pos]->convert_and_push_back(name);
        }
    }
}

DataFrame::DataFrame(std::ifstream& file)
    : columns(), index_names(), column_names()
{
    vector<string> colNames = create_column_names(file);
    initialize_column(file, colNames);
    insert_data(file, colNames);
}
