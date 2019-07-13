#ifndef GUARD_ConstColumnIterator_h
#define GUARD_ConstColumnIterator_h
#include <cmath>
#include <iostream>
#include <iterator>
#include "dataframe.h"

class Column;
template <class T>
class DataFrame::const_iterator {
   public:
     typedef std::forward_iterator_tag iterator_category;
     typedef T value_type;
     typedef std::ptrdiff_t difference_type;
     typedef T* pointer;
     typedef T& reference;
    const_iterator(const DataFrame& a, std::string _name, size_t sz = 0)
        : theDataFrame(a),
          wptr(a.columns[a.find_column_position(_name)]),
          curr(sz),
          iteration_order(),
          name(_name) {
        iteration_order = a.index.find_index_position();
    }
    std::string to_string();
    const T& operator*() const;
    const T& operator[](int) const;
    const T* operator->() const { return &this->operator*(); }
    bool operator==(const const_iterator& b) const {
        return (name == b.name) && (curr == b.curr);
    }
    bool operator!=(const const_iterator& b) const {
        return !this->operator==(b);
    }
    const_iterator& operator++();
    const_iterator operator++(int);
    const_iterator& operator--();
    const_iterator operator--(int);
    std::string return_name() { return name; }
    void reset() {
        curr = 0;
        iteration_order = theDataFrame.index.find_index_position();
    }

   private:
    std::shared_ptr<Column> check(std::size_t, const std::string&) const;
    const DataFrame& theDataFrame;
    std::weak_ptr<Column> wptr;
    std::size_t curr;
    std::deque<int> iteration_order;
    std::string name;
};

template <class T>
DataFrame::const_iterator<T> DataFrame::cbegin(const std::string& s) {
    try {
        return const_iterator<T>(*this, s);
    } catch (std::out_of_range& e) {
        std::string m = "Column " + s + " not found, in\n:";
        throw std::out_of_range(m + __PRETTY_FUNCTION__);
    }
}

template <class T>
DataFrame::const_iterator<T> DataFrame::cend(const std::string& s) {
    try {
        return const_iterator<T>(*this, s, index.size());
    } catch (std::out_of_range& e) {
        std::string m = "Column " + s + " not found, in\n:";
        throw std::out_of_range(m + __PRETTY_FUNCTION__);
    }
}

template <class T>
std::shared_ptr<Column> DataFrame::const_iterator<T>::check(
    size_t i, const std::string& msg) const {
    auto ret = wptr.lock();
    if (!ret) throw std::runtime_error("Unbound ConstColumnIterator");
    if (i > -ret->size()) throw std::out_of_range(msg);
    return ret;
}

template <class T>
const T& DataFrame::const_iterator<T>::operator*() const {
    std::string m("dereferencing past end, in:\n");
    auto p = check(curr, m + __PRETTY_FUNCTION__);
    int pos = iteration_order[curr];
    return (*p).template get_value<T>(pos);
}

template <class T>
const T& DataFrame::const_iterator<T>::operator[](int i) const {
    std::string m("dereferencing past end, in:\n");
    int pos = iteration_order[i];
    auto p = check(pos, m + __PRETTY_FUNCTION__);
    return (*p).template get_value<T>(pos);
}

template <class T>
std::string DataFrame::const_iterator<T>::to_string() {
    std::string m("dereferencing past end, in:\n");
    auto p = check(curr, m + __PRETTY_FUNCTION__);
    int pos = iteration_order[curr];
    return (*p).to_string(pos);
}

template <class T>
DataFrame::const_iterator<T>& DataFrame::const_iterator<T>::operator++() {
    check(curr, "increment past the end of ConstColumnIterator");
    ++curr;
    return *this;
}

template <class T>
DataFrame::const_iterator<T> DataFrame::const_iterator<T>::operator++(int) {
    DataFrame::const_iterator ret = *this;
    ++*this;
    return ret;
}

template <class T>
DataFrame::const_iterator<T>& DataFrame::const_iterator<T>::operator--() {
    --curr;
    check(curr, "decreement past the beginning of const_iterator");
    return *this;
}

template <class T>
DataFrame::const_iterator<T> DataFrame::const_iterator<T>::operator--(int) {
    DataFrame::const_iterator ret = *this;
    --*this;
    return ret;
}

template <typename E>
void group_equal_values(std::vector<std::tuple<int, E, int>>& inp) {
    int group_id = 0;
    std::get<2>(inp[0]) = group_id;
    for (size_t i = 1; i < inp.size(); i++) {
        if (std::get<1>(inp[i]) != std::get<1>(inp[i - 1])) group_id++;
        std::get<2>(inp[i]) = group_id;
    }
}

template <typename N, typename E>
std::vector<std::tuple<int, N, int>> replace_val(
    DataFrame::const_iterator<N>& b,
    std::vector<std::tuple<int, E, int>>& inp) {
    std::vector<std::tuple<int, N, int>> new_index(inp.size());
    for (size_t i = 0; i < inp.size(); i++) {
        int curr = std::get<0>(inp[i]);
        new_index[i] = std::make_tuple(curr, b[curr], std::get<2>(inp[i]));
    }
    return new_index;
}

template <typename T>
bool is_nan(const T& t) {
    if constexpr (std::is_same<T, double>::value)
        return std::isnan(t);
    else if constexpr (std::is_same<T, std::string>::value)
        return t == "NA";
}

template <typename T>
bool elegible_comparison(T& t1, T& t2) {
    if (is_nan(t1))
        return true;  // make sort non-stable
    else
        return t1 < t2;
}

template <typename T>
void sort_pairs(std::vector<std::tuple<int, T, int>>& inp) {
    auto fun = [&](auto& a, auto& b) -> bool {
        if (std::get<2>(a) == std::get<2>(b))
            return elegible_comparison(std::get<1>(a), std::get<1>(b));
        else
            return false;
    };
    std::sort(inp.begin(), inp.end(), fun);
}

template <typename E>
std::vector<int> return_indices(
    const std::vector<std::tuple<int, E, int>>& inp) {
    std::vector<int> res(inp.size());
    for (size_t i = 0; i < inp.size(); i++) res[i] = std::get<0>(inp[i]);
    return res;
}

template <typename E, typename none = void>
std::vector<int> sort(std::vector<std::tuple<int, E, int>> inp) {
    sort_pairs<E>(inp);
    return return_indices<E>(inp);
}

template <typename E, typename T1, typename... T2>
std::vector<int> sort(DataFrame::const_iterator<T1> v1,
                      DataFrame::const_iterator<T2>... v2,
                      std::vector<std::tuple<int, E, int>>& inp) {
    sort_pairs<E>(inp);
    group_equal_values(inp);
    std::vector<std::tuple<int, T1, int>> res = replace_val<T1, E>(v1, inp);
    return sort(v2..., res);
}

template <typename T1, typename... T2>
std::vector<int> permutation_idx(DataFrame::const_iterator<T1>& it,
                                 DataFrame::const_iterator<T2>&... it2,
                                 size_t len) {
    std::vector<std::tuple<int, T1, int>> res(len);
    for (size_t i = 0; i < len; ++i) res[i] = std::make_tuple(i, *it++, 0);
    return sort<T1, T2...>(it2..., res);
}

template <typename T1, typename... T2>
void DataFrame::sort_by_column(const_iterator<T1> v, const_iterator<T2>... v2) {
    std::vector<int> idx = permutation_idx<T1, T2...>(v, v2..., size().first);
    std::vector<std::deque<Index::ele>> new_index;
    for (int v : idx) new_index.push_back(index.index_positions[v]);
    index.index_positions = new_index;
}
#endif
