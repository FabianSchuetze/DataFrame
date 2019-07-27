#ifndef GUARD_ColumnIterator_h
#define GUARD_ColumnIterator_h
#include <cmath>
#include <iostream>
#include "dataframe.h"

class Column;
template <class T>
class DataFrame::iterator {
   public:
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef T value_type;
    typedef std::ptrdiff_t difference_type;
    typedef T* pointer;
    typedef T& reference;
    template <typename T1, typename... T2>
    friend void DataFrame::test_belonging(iterator<T1>& v, iterator<T2>&... v2);
    iterator() = default;
    iterator& operator=(const iterator& a) = default;
    iterator(DataFrame* a, std::string _name, size_t sz = 0)
        : theDataFrame(a),
          wptr(a->columns[a->find_column_position(_name)]),
          curr(sz),
          iteration_order(),
          name(_name) {
        iteration_order = a->index.find_index_position();
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
    std::shared_ptr<Column> check(std::size_t, const std::string&,
                                  const char*) const;
    DataFrame* theDataFrame;
    std::weak_ptr<Column> wptr;
    std::size_t curr;
    std::deque<int> iteration_order;
    std::string name;
};

template <class T>
DataFrame::iterator<T> DataFrame::begin(const std::string& s) {
    try {
        make_unique_if(s);
        return iterator<T>(this, s);
    } catch (std::out_of_range& e) {
        std::string m = "Column " + s + " not found, in\n:";
        throw std::out_of_range(m + __PRETTY_FUNCTION__);
    }
}

template <class T>
DataFrame::iterator<T> DataFrame::end(const std::string& s) {
    try {
        make_unique_if(s);
        return iterator<T>(this, s, index.size().first);
    } catch (std::out_of_range& e) {
        std::string m = "Column " + s + " not found, in\n:";
        throw std::out_of_range(m + __PRETTY_FUNCTION__);
    }
}

template <class T>
std::shared_ptr<Column> DataFrame::iterator<T>::check(size_t i,
                                                      const std::string& msg,
                                                      const char* fun) const {
    auto ret = wptr.lock();
    if (!ret) {
        std::string m = std::string("Unbound iter, in\n") + __PRETTY_FUNCTION__;
        std::string m2 = std::string("\nCalled from:\n") + fun;
        throw std::runtime_error(m + m2);
    }
    if (i >= ret->size()) {
        std::string m = msg + std::string(" in:\n") + __PRETTY_FUNCTION__;
        std::string m2 = std::string("\nCalled from:\n") + fun;
        throw std::out_of_range(m + m2);
    }
    return ret;
}

template <class T>
T& DataFrame::iterator<T>::operator*() const {
    auto p = check(curr, "dereferencing past end", __PRETTY_FUNCTION__);
    int pos = iteration_order[curr];
    return (*p).template get_value<T>(pos);
}

template <class T>
T& DataFrame::iterator<T>::operator[](int i) {
    auto p = check(i, "dereferencing past end", __PRETTY_FUNCTION__);
    int pos = iteration_order[i];
    return (*p).template get_value<T>(pos);
}

template <class T>
DataFrame::iterator<T>& DataFrame::iterator<T>::operator++() {
    check(curr, "increment past the end of iterator", __PRETTY_FUNCTION__);
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
    check(curr, "decreement past the beginning of iterator",
          __PRETTY_FUNCTION__);
    return *this;
}

template <class T>
DataFrame::iterator<T> DataFrame::iterator<T>::operator--(int) {
    DataFrame::iterator ret = *this;
    --*this;
    return ret;
}

template <typename T>
void DataFrame::fill_na(std::string s, T t) {
    try {
        iterator<T> it = begin<T>(s);
        iterator<T> e = end<T>(s);
        std::transform(it, e, it, [&t](auto& d) { return std::isnan(d) ? t : d; });
    } catch (const std::out_of_range&) {
        std::string m("Could not find column " + s + "in \n:");
        throw std::out_of_range(s + __PRETTY_FUNCTION__);
    }
}

template <typename T1, typename... T2>
void DataFrame::test_belonging(iterator<T1>& v, iterator<T2>&... v2) {
    bool eq = this == v.theDataFrame;
    if (!eq) {
        std::string m("The iterator points to a different dataframe, in:\n");
        throw std::invalid_argument(m + __PRETTY_FUNCTION__);
    }
    if constexpr (sizeof...(T2) > 0) test_belonging<T2...>(v2...);
}

#endif
