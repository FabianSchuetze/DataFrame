#include "index.h"

using std::deque;
using std::string;
using std::vector;
void Index::append_row(const deque<ele>& row) {
    index_names[row].push_back(index_positions.size());
    index_positions.push_back(row);
}
template <typename T>
void Index::append_index_column(const vector<T>& inp) {
    if (inp.size() != index_positions.size())
        throw std::invalid_argument("Cant do it in " + __PRETTY_FUNCTION__);
    std::map<deque<ele>, deque<int>> new_index_names;
    int i = 0;
    for (auto& tmp : index_positions) {
        append_value(inp[i], tmp);
        new_index_names[tmp].push_back(i++);
    }
    index_names = new_index_names;
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
deque<int> Index::find_index_position(const std::deque<ele>& inp) {
    return index_names.at(inp);
}

void Index::remove_value(size_t pos, std::deque<ele>& t) {
    t.erase(t.begin() + pos);
}

template <typename T>
void Index::append_value(const T& t, std::deque<ele>& set) {
    set.push_back(t);
}
