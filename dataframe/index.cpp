#include "index.h"

using std::deque;
using std::string;
using std::vector;

void Index::append_row(const deque<ele>& row) {
    index_names[row].push_back(index_positions.size());
    index_positions.push_back(row);
}
void Index::drop_index(size_t pos) {
    std::map<deque<ele>, deque<int>> new_index_names;
    int i = 0;
    for (auto& tmp : index_positions) {
        remove_value(pos, tmp);
        new_index_names[tmp].push_back(i++);
    }
    index_names = new_index_names;
}
deque<int> Index::find_index_position(const deque<ele>& inp) const {
    try {
        return index_names.at(inp);
    } catch (std::out_of_range& e) {
        return deque<int>();
    }
}

deque<int> Index::find_index_position(const deque<ele>& inp) {
    return static_cast<const Index&>(*this).find_index_position(inp);
}

void Index::remove_value(size_t pos, std::deque<ele>& t) {
    t.erase(t.begin() + pos);
}

int pop_and_return(const deque<Index::ele>& val, Index::index_map_type& inp) {
    int pos = inp.at(val).front();
    inp.at(val).pop_front();
    return pos;
}

deque<int> Index::find_index_position() const {
    std::map<deque<ele>, deque<int>> copy_index_names(index_names);
    deque<int> res;
    for (const auto& s : index_positions)
        res.push_back(pop_and_return(s, copy_index_names));
    return res;
}

deque<int> Index::find_index_position() {
    return static_cast<const Index&>(*this).find_index_position();
}

vector<deque<Index::ele>> Index::get_index_names() { return index_positions; }
vector<deque<Index::ele>> Index::get_index_names() const {
    return index_positions;
}

void Index::append_index(const deque<ele>& s) {
    index_names[s].push_back(index_positions.size());
    index_positions.push_back(s);
}

void Index::append_index(const deque<ele>& s, int pos) {
    index_names[s].push_back(pos);
    index_positions.push_back(s);
}

void Index::append_index(const vector<deque<ele>>& idx) {
    for (const auto& s : idx) append_index(s);
}

std::set<deque<Index::ele>> Index::unique_elements() const {
    auto b = index_positions.begin();
    auto e = index_positions.end();
    return std::set<deque<Index::ele>>(b, e);
}

std::set<deque<Index::ele>> Index::unique_elements() {
    return static_cast<const Index&>(*this).unique_elements();
}
std::string to_string(Index::ele e) {
    if (const int* val = std::get_if<int>(&e))
        return std::to_string(*val);
    else if (const string* val = std::get_if<string>(&e))
        return *val;
    else if (const double* val = std::get_if<double>(&e))
        return std::to_string(*val);
    else {
        std::string m = "Cannot convert value, in:\n";
        throw std::invalid_argument(m + __PRETTY_FUNCTION__);
    }
}

string convert_deque_to_string(const deque<Index::ele>& inp) {
    string res("");
    for (const Index::ele& e : inp) res += to_string(e) + ", ";
    return string(res.begin(), res.end() - 2);
}

vector<string> Index::get_index_as_string() const {
    vector<string> res(index_positions.size());
    int i = 0;
    for (auto const& val : index_positions)
        res[i++] = convert_deque_to_string(val);
    return res;
}

std::vector<std::string> Index::get_index_as_string() {
    return static_cast<const Index&>(*this).get_index_as_string();
}

void carthesian_product(deque<int>& lhs, deque<int>& rhs,
                        deque<std::pair<int, int>>& inp) {
    if (rhs.empty()) rhs.push_back(-1);
    for (const int& a : lhs)
        for (const int& b : rhs) inp.push_back(std::make_pair(a, b));
}
deque<std::pair<int, int>> Index::index_correspondence(const Index& rhs) const {
    deque<std::pair<int, int>> res;
    for (auto const& e : unique_elements()) {
        deque<int> lhsIdx = find_index_position(e);
        deque<int> rhsIdx = rhs.find_index_position(e);
        carthesian_product(lhsIdx, rhsIdx, res);
    }
    return res;
}

deque<std::pair<int, int>> Index::index_correspondence(const Index& rhs) { 
    return static_cast<const Index&>(*this).index_correspondence(rhs);
}
