#include "index.h"

using std::deque;
using std::string;
using std::vector;
void Index::append_row(const deque<ele>& row) {
    index_names[row].push_back(index_positions.size());
    index_positions.push_back(row);
}
//template <typename T>
//void Index::append_index_column(const vector<T>& inp) {
    //if (inp.size() != index_positions.size())
        //throw std::invalid_argument("Cant do it in " + __PRETTY_FUNCTION__);
    //std::map<deque<ele>, deque<int>> new_index_names;
    //int i = 0;
    //for (auto& tmp : index_positions) {
        //append_value(inp[i], tmp);
        //new_index_names[tmp].push_back(i++);
    //}
    //index_names = new_index_names;
//}
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

//template <typename T>
//void Index::append_value(const T& t, std::deque<ele>& set) {
    //set.push_back(t);
//}

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
void Index::append_index(const int& t) {
    deque<ele> s((t));
    index_names[s].push_back(index_positions.size());
    index_positions.push_back(s);
}
void Index::append_index(const string& t) {
    deque<ele> s({t});
    index_names[s].push_back(index_positions.size());
    index_positions.push_back(s);
}

void Index::append_index(const vector<deque<ele>>& idx) {
    for (const auto& s : idx) append_index(s);
}
void Index::append_index(const std::vector<int>& inp) {
    for (const int& t : inp) append_index(t);
}
void Index::append_index(const std::vector<std::string>& inp) {
    for (const std::string& t : inp) append_index(t);
}

std::set<deque<Index::ele>> Index::unique_elements() const {
    auto b = index_positions.begin();
    auto e = index_positions.end();
    return std::set<deque<Index::ele>>(b, e);
}

std::set<deque<Index::ele>> Index::unique_elements() {
    return static_cast<const Index&>(*this).unique_elements();
}
