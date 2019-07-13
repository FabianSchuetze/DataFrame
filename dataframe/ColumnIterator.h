#ifndef GUARD_ColumnIterator_h
#define GUARD_ColumnIterator_h
#include <cmath>
#include <iostream>
#include "dataframe.h"

class Column;
template <class T>
class DataFrame::iterator {
   public:
    typedef std::input_iterator_tag iterator_category;
    typedef T value_type;
    typedef std::ptrdiff_t difference_type;
    typedef T* pointer;
    typedef T& reference;
    iterator(DataFrame& a, std::string _name, size_t sz = 0)
        : theDataFrame(a),
          wptr(a.columns[a.find_column_position(_name)]),
          curr(sz),
          iteration_order(),
          name(_name) {
        iteration_order = a.index.find_index_position();
    }
    T& operator*() const;
    T& operator[](int);
    iterator& operator++();
    iterator operator++(int);
    iterator& operator--();
    iterator operator--(int);
    bool operator==(const iterator& b) const {
        return (name == b.name) && (curr == b.curr);
    }
    bool operator!=(const iterator& b) const { return !this->operator==(b); }
    const T* operator->() const { return &this->operator*(); }
    std::string return_name() { return name; }

   private:
    std::shared_ptr<Column> check(std::size_t, const std::string&) const;
    DataFrame& theDataFrame;
    std::weak_ptr<Column> wptr;
    std::size_t curr;
    std::deque<int> iteration_order;
    std::string name;
};

template <class T>
DataFrame::iterator<T> DataFrame::begin(const std::string& s) {
    try {
        make_unique_if(s);
        return iterator<T>(*this, s);
    } catch (std::out_of_range& e) {
        std::string m = "Column " + s + " not found, in\n:";
        throw std::out_of_range(m + __PRETTY_FUNCTION__);
    }
}

template <class T>
DataFrame::iterator<T> DataFrame::end(const std::string& s) {
    try {
        make_unique_if(s);
        return iterator<T>(*this, s, index.size());
    } catch (std::out_of_range& e) {
        std::string m = "Column " + s + " not found, in\n:";
        throw std::out_of_range(m + __PRETTY_FUNCTION__);
    }
}

template <class T>
std::shared_ptr<Column> DataFrame::iterator<T>::check(
    size_t i, const std::string& msg) const {
    auto ret = wptr.lock();
    if (!ret) throw std::runtime_error("Unbound iterator");
    if (i > -ret->size()) throw std::out_of_range(msg);
    return ret;
}

template <class T>
T& DataFrame::iterator<T>::operator*() const {
    auto p = check(curr, "dereferencing past end");
    int pos = iteration_order[curr];
    return (*p).template get_value<T>(pos);
}

template <class T>
T& DataFrame::iterator<T>::operator[](int i) {
    auto p = check(i, "dereferencing past end");
    int pos = iteration_order[i];
    return (*p).template get_value<T>(pos);
}

template <class T>
DataFrame::iterator<T>& DataFrame::iterator<T>::operator++() {
    check(curr, "increment past the end of iterator");
    ++curr;
    return *this;
}

template <class T>
DataFrame::iterator<T> DataFrame::iterator<T>::operator++(int) {
    DataFrame::iterator ret = *this;
    ++*this;
    return ret;
}

template <class T>
DataFrame::iterator<T>& DataFrame::iterator<T>::operator--() {
    --curr;
    check(curr, "decreement past the beginning of iterator");
    return *this;
}

template <class T>
DataFrame::iterator<T> DataFrame::iterator<T>::operator--(int) {
    DataFrame::iterator ret = *this;
    --*this;
    return ret;
}
// CAN IS USE REPLACE!?!
template <typename T>
void DataFrame::fill_na(std::string s, T t) {
    iterator<T> it = begin<T>(s);
    iterator<T> e = end<T>(s);
    std::transform(it, e, it, [&t](auto& d) { return std::isnan(d) ? t : d; });
}

#endif
