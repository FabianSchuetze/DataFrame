#include "dataframe.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <numeric>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <unordered_set>
#include <set>
#include <unordered_map>
#include "ConstColumnIterator.h"
#include "dataframeproxy.h"
class ColumnIterator;
using std::make_pair;
using std::make_shared;
using std::pair;
using std::shared_ptr;
using std::string;
using std::vector;
using std::deque;

void missing_col_error(const char* what, string s) {
    std::string msg = " Column " + s + " not found";
    throw std::out_of_range(what + msg);
}

deque<int> DataFrame::find_index_position() const {
    std::unordered_map<string, deque<int>>copy_index_names(index_names);
    deque<int>res;
    for (const string& s: index_positions) {
        int pos = copy_index_names.at(s).front();
        copy_index_names.at(s).pop_front();
        res.push_back(pos);
    }
    return res;
}
deque<int> DataFrame::find_index_position() {
    return static_cast<const DataFrame&>(*this).find_index_position();
}

deque<int> DataFrame::find_index_position(const vector<string>& inp) const {
    std::set<string>s(inp.begin(),inp.end());
    if (s.size() < inp.size())
        std::cerr << "Input vector has duplicates, result has duplicates\n";
    deque<int> res;
    for (const string& s: inp) {
        deque<int> tmp = find_index_position(s);
        std::copy(tmp.begin(), tmp.end(), std::back_inserter(res));
    }
    return res;
}

deque<int> DataFrame::find_index_position(const string& s) const {
    try {
        return index_names.at(s);
    } catch (std::out_of_range& e) {
        return deque<int>();
    }
}

deque<int> DataFrame::find_index_position(const std::string& s) {
    return static_cast<const DataFrame&>(*this).find_index_position(s);
}
template <typename T>
vector<string> get_names(T& cont) {
    vector<string> res;
    std::transform(cont.begin(), cont.end(), std::back_inserter(res),
                   [](const pair<string, int>& ele) { return ele.first; });
    return res;
}

vector<string> DataFrame::get_index_names() { return index_positions; }
vector<string> DataFrame::get_index_names() const { return index_positions; }

vector<string> DataFrame::get_column_names() { return get_names(column_names); }
vector<string> DataFrame::get_column_names() const {
    return get_names(column_names);
}

DataFrame::DataFrame()
    : columns(), index_names(), index_positions(), column_names() {}

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
        throw std::out_of_range("No col with name" + s);
    }
}

void DataFrame::make_contigious() {
    DataFrame new_df = deep_copy(*this);
    *this = new_df;
}

DataFrame deep_copy(const DataFrame& lhs) {
    DataFrame new_df = DataFrame();
    deque<int> old_positions = lhs.find_index_position();
    for (auto const& x : lhs.column_names) {
        new_df.column_names[x.first] = new_df.column_names.size();
        new_df.columns.push_back(lhs.get_unique(x.first, old_positions));
    }
    new_df.append_index(lhs.index_positions);
    return new_df;
}

void DataFrame::append_index(const vector<string>& idx) {
    for (const string& s: idx)
        append_index(s);
}

void DataFrame::append_index(const string& s) {
    index_names[s].push_back(index_positions.size());
    index_positions.push_back(s);
}

template <typename T>
DataFrame::DataFrame(const vector<string>& idx, const vector<string>& n,
                     const vector<vector<T>>& cols) {
    if (n.size() != cols.size()) {
        string s("#of Input vector differs from # of colum names, in: ");
        throw std::invalid_argument(s + __PRETTY_FUNCTION__);
    }
    for (size_t i = 0; i < n.size(); ++i) {
        if (idx.size() != cols[i].size())
            throw std::invalid_argument("Data and index has different length");
        columns.push_back(make_shared<Column>(cols[i]));
        column_names[n[i]] = i;
    }
    append_index(idx);
}
template DataFrame::DataFrame(const vector<string>&, const vector<string>&,
                              const vector<vector<double>>&);
template DataFrame::DataFrame(const vector<string>&, const vector<string>&,
                              const vector<vector<string>>&);
template DataFrame::DataFrame(const vector<string>&, const vector<string>&,
                              const vector<vector<bool>>&);

DataFrame::DataFrame(const DataFrame::DataFrameProxy& df)
    : columns(), index_names(), index_positions(df.idxNames), column_names() {
    for (const string& name : df.colNames) {
        columns.push_back(df.theDataFrame.get_shared_copy(name));
        column_names[name] = column_names.size();
    }
    for (const string& name : df.idxNames) {
        deque<int> pos = df.theDataFrame.find_index_position(name);
        index_names[name] = pos;
    }
}

std::pair<size_t, size_t> DataFrame::size() const {
    return make_pair(index_positions.size(), columns.size());
}

int DataFrame::use_count(const string& name) {
    int pos = find_column_position(name);
    return columns[pos].use_count();
}

void carthesian_product(deque<int>& lhs, deque<int>& rhs, 
                        deque<pair<int, int>>&inp) {
    if (rhs.empty()) rhs.push_back(-1);
    for (const int& a : lhs)
        for (const int& b : rhs)
            inp.push_back(make_pair(a,b));
}

std::set<string> unique_names(const vector<string>& inp) {
    std::set<string>s(inp.begin(), inp.end());
    return s;
}

deque<pair<int, int>> correspondence_position(const DataFrame& lhs,
                                               const DataFrame& other) {
    deque<pair<int, int>> res;
    for (const string& s : unique_names(lhs.index_positions)) {
        deque<int> lhsIdx = lhs.find_index_position(s);
        deque<int> rhsIdx = other.find_index_position(s);
        carthesian_product(lhsIdx, rhsIdx, res);
    }
    return res;
}

void DataFrame::copy_row(int pos) {
    for (auto const x : column_names) {
        std::cout << x.first << std::endl;
        columns[x.second]->copy_row(pos);
    }
}

void DataFrame::append_duplicate_rows(int pos) {
    append_index(index_positions[pos]);
    copy_row(pos);
}

void DataFrame::append_duplicate_rows(deque<pair<int, int>>& indices) {
    std::unordered_set<int> s;
    for (auto& pair: indices) {
        if (s.count(pair.first)) {
            append_duplicate_rows(pair.first);
            pair.first = index_positions.size() - 1;
        }
        s.insert(pair.first);
    }
}

void DataFrame::append_nan_rows() {
    for (auto& x : column_names) {
        std::cout << "before: " << columns[x.second]->size() << std::endl;
        columns[x.second]->push_back_nan();
        std::cout << "after: " << columns[x.second]->size() << std::endl;
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

void DataFrame::drop_row(const string& s) {
    vector<string> tmp{s};
    return drop_row(tmp);
}

void DataFrame::drop_row(vector<string> vec) {
    std::sort(vec.begin(), vec.end());
    auto fun = [&](const string& val) {
        return !std::binary_search(vec.begin(), vec.end(), val);
    };
    auto it = std::stable_partition(index_positions.begin(),
                                    index_positions.end(), fun);
    index_positions.erase(it, index_positions.end());
    for (string& s : vec) index_names.erase(s);
}

void DataFrame::dropna() {
    vector<int> count = contains_null();
    auto fun = [&](const string& s) -> bool {
        int pos = find_index_position(s).front();
        index_names[s].pop_front();
        if (count[pos] > 0)
            return true;
        index_names[s].push_back(pos);
        return false;
    };
    auto new_end = std::remove_if(index_positions.begin(),
            index_positions.end(), fun);
    index_positions.erase(new_end, index_positions.end());
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
    // WHAT HAPPENS IF THERE IS ONLY ONE SHARED POINTER OF IT? DOES THE THINK
    // DROP OUT OF THE VECTOR?
    columns[find_column_position(s)].~shared_ptr();  // reduce use_count()
    column_names.erase(s);                          // delete reference to it;
}

void DataFrame::sort_by_index() {
    std::sort(index_positions.begin(), index_positions.end(),
              [](auto& a, auto& b) { return a < b; });
}

void DataFrame::sort_by_column(const std::string& s) {
    string type = columns[find_column_position(s)]->type_name();
    if (type == "double")
        sort_by_column_template<double>(s);
    else if (type == "string")
        sort_by_column_template<string>(s);
    else if (type == "bool") {
        string msg = "Column: " + s + " is type bool, no sort implemented";
        throw std::logic_error(msg);
    }
}

template <typename T>
void DataFrame::sort_by_column_template(const string& s) {
    std::vector<string> new_index;
    vector<int> argsort = permutation_index<T>(s);
    for (size_t i = 0; i < argsort.size(); ++i) {
        string name = index_positions[argsort[i]];
        new_index.push_back(name);
    }
    index_positions = new_index;
}

template void DataFrame::sort_by_column_template<double>(const std::string&);
template void DataFrame::sort_by_column_template<std::string>(
    const std::string&);

bool DataFrame::is_contigious() {
    // HERE THE FUNCTION WOULD STILL RETURN THE SAME INTERFACE
    deque<int> existing_order = find_index_position();
    for (size_t i = 1; i < existing_order.size(); ++i)
        if ((existing_order[i] - existing_order[i - 1]) != 1) return false;
    return true;
}
void DataFrame::convert_bool_to_double(const std::string& s) {
    int pos = find_column_position(s);
    columns[pos]->convert_bool_to_double();
}

vector<string> DataFrame::create_column_names(std::ifstream& file) {
    std::string line, name;
    std::getline(file, line);
    size_t i = 0;
    vector<string> cols;
    std::istringstream names(line);
    std::getline(names, name, ',');  // pass the index column
    while (std::getline(names, name, ',')) {
        cols.push_back(name);
        try {
            column_names.at(name);
            throw std::invalid_argument("Duplicate column name " + name);
        } catch (std::out_of_range& e) {
            column_names[name] = i++;
        }
    }
    return cols;
}

template <typename T>
void DataFrame::initialize_column(const string& s) {
    Column col = Column(vector<T>());
    columns[find_column_position(s)] = make_shared<Column>(col);
}

void DataFrame::initialize_column(std::ifstream& file,
                                  const vector<string>& cols) {
    columns.resize(cols.size());
    std::string line, name;
    std::getline(file, line);
    std::istringstream names(line);
    std::getline(names, name, ',');  // pass the index column
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
            throw std::invalid_argument("Input type: " + name +
                                        " incompatible");
    }
}

void DataFrame::insert_data(std::ifstream& file, const vector<string>& cols) {
    std::string line, name;
    while (std::getline(file, line)) {
        std::istringstream names(line);
        std::getline(names, name, ',');
        append_index(name);
        for (const string& col: cols) {
            std::getline(names, name, ',');
            columns[find_column_position(col)]->convert_and_push_back(name);
        }
    }
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
    : columns(), index_names(), index_positions(), column_names() {
    vector<string> colNames = create_column_names(file);
    initialize_column(file, colNames);
    insert_data(file, colNames);
    assert_same_column_length(__PRETTY_FUNCTION__);
    //for (size_t i = 1; i < colNames.size(); ++i) {
        //if (columns[0]->size() != columns[i]->size()) {
            //string msg("Column " + colNames[i] + " has different len");
            //throw std::runtime_error(msg);
        //}
    //}
}
