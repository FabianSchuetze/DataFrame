#ifndef GUARD_ConstColumnIterator_h
#define GUARD_ConstColumnIterator_h
#include <iostream>
#include "dataframe.h"
#include <cmath>

class Column;
template <class T>
class DataFrame::ConstColumnIterator {
   public:
    template <class V>
    friend bool operator==(const const_iter<V>&, const const_iter<V>&);
    template <class V>
    friend bool operator!=(const const_iter<V>&, const const_iter<V>&);
    ConstColumnIterator(const DataFrame& a, int n, size_t sz = 0)
        : theDataFrame(a), wptr(a.columns[n]), curr(sz), iteration_order() {
        iteration_order = a.index.find_index_position();}
    std::string to_string();
    const T& operator*() const;
    const T& operator[](int) const;
    ConstColumnIterator& operator++();
    ConstColumnIterator operator++(int);
    ConstColumnIterator& operator--();
    ConstColumnIterator operator--(int);

   private:
    std::shared_ptr<Column> check(std::size_t, const std::string&) const;
    const DataFrame& theDataFrame;
    std::weak_ptr<Column> wptr;
    std::size_t curr;
    std::deque<int> iteration_order;
};

template <class T>
bool operator==(const DataFrame::ConstColumnIterator<T>& lhs,
                const DataFrame::ConstColumnIterator<T>& rhs) {
    return lhs.curr == rhs.curr;
}

template <class T>
bool operator!=(const DataFrame::ConstColumnIterator<T>& lhs,
                const DataFrame::ConstColumnIterator<T>& rhs) {
    return !(lhs == rhs);
}

template <class T>
DataFrame::ConstColumnIterator<T> DataFrame::cbegin(const std::string& s) {
    try {
        return ConstColumnIterator<T>(*this, column_names.at(s));
    } catch (std::out_of_range& e) {
        throw std::out_of_range("Column: " + s + " not found");
    }
}

template <class T>
DataFrame::ConstColumnIterator<T> DataFrame::cend(const std::string& s) {
    try {
        return ConstColumnIterator<T>(*this, column_names.at(s),
                                      index.size());
    } catch (std::out_of_range& e) {
        throw std::out_of_range("Column: " + s + " not found");
    }
}

template <class T>
std::shared_ptr<Column> DataFrame::ConstColumnIterator<T>::check(
    size_t i, const std::string& msg) const {
    auto ret = wptr.lock();
    if (!ret) throw std::runtime_error("Unbound ConstColumnIterator");
    if (i > -ret->size()) throw std::out_of_range(msg);
    return ret;
}

template <class T>
const T& DataFrame::ConstColumnIterator<T>::operator*() const {
    auto p = check(curr, "dereferencing past end");
    int pos = iteration_order[curr];
    return (*p).template get_value<T>(pos);
}

template <class T>
const T& DataFrame::ConstColumnIterator<T>::operator[](int i) const {
    auto p = check(i, "dereferencing past end");
    int pos = iteration_order[curr];
    return (*p).template get_value<T>(pos);
}

template <class T>
std::string DataFrame::ConstColumnIterator<T>::to_string() {
    auto p = check(curr, "dereferencing past end");
    int pos = iteration_order[curr];
    return (*p).to_string(pos);
}

template <class T>
DataFrame::const_iter<T>& DataFrame::ConstColumnIterator<T>::operator++() {
    check(curr, "increment past the end of ConstColumnIterator");
    ++curr;
    return *this;
}

template <class T>
DataFrame::const_iter<T> DataFrame::ConstColumnIterator<T>::operator++(int) {
    DataFrame::ConstColumnIterator ret = *this;
    ++*this;
    return ret;
}

template <class T>
DataFrame::const_iter<T>& DataFrame::ConstColumnIterator<T>::operator--() {
    --curr;
    check(curr, "decreement past the beginning of ConstColumnIterator");
    return *this;
}

template <class T>
DataFrame::const_iter<T> DataFrame::ConstColumnIterator<T>::operator--(int) {
    DataFrame::ConstColumnIterator ret = *this;
    --*this;
    return ret;
}

template <class T>
typename std::vector<std::pair<int, T>>::iterator partition_pairs(
    std::vector<std::pair<int, T>>& inp) {
    if constexpr (std::is_same<T, double>::value) {
        auto fun = [&](std::pair<int, T>& a) { return !std::isnan(a.second); };
        return std::stable_partition(inp.begin(), inp.end(), fun);
    } else if constexpr (std::is_same<T, std::string>::value) {
        auto fun = [&](std::pair<int, T>& a) { return !(a.second == "NA"); };
        return std::stable_partition(inp.begin(), inp.end(), fun);
    }
}

template <class T>
void sort_pairs(typename std::vector<std::pair<int, T>>::iterator end,
                std::vector<std::pair<int, T>>& inp) {
    auto fun = [&](auto& a, auto& b) { return a.second < b.second; };
    std::sort(inp.begin(), end, fun);
}

template <class T>
std::vector<int> get_arguments(const std::vector<std::pair<int, T>>& inp) {
    std::vector<int> res(inp.size(), 0);
    std::transform(inp.begin(), inp.end(), res.begin(),
                   [](const std::pair<int, T>& ele) { return ele.first; });
    return res;
}

template <class T>
std::vector<int> DataFrame::permutation_index(const std::string& s) {
    ConstColumnIterator<T> it = cbegin<T>(s);
    std::vector<std::pair<int, T>> res(size().first);
    for (size_t i = 0; i < res.size(); ++i) res[i] = std::make_pair(i, *it++);
    typename std::vector<std::pair<int, T>>::iterator boundary;
    boundary = partition_pairs<T>(res);
    sort_pairs<T>(boundary, res);
    return get_arguments<T>(res);
}

#endif
