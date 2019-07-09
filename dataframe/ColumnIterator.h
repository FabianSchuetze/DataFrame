#ifndef GUARD_ColumnIterator_h
#define GUARD_ColumnIterator_h
#include "dataframe.h"
#include <iostream>
#include <cmath>

class Column;
template <class T>
class DataFrame::ColumnIterator {
    public:
        template <class V>
        friend bool operator==(const iter<V>&, const iter<V>&);
        template <class V>
        friend bool operator!=(const iter<V>&, const iter<V>&);
        //ColumnIterator();
        ColumnIterator(DataFrame& a, std::string _name, size_t sz=0):
            theDataFrame(a), 
          wptr(a.columns[a.find_column_position(_name)]),
          curr(sz),
          iteration_order(),
          name(_name) {
        iteration_order = a.index.find_index_position();
    }
        T& operator*() const;
        T& operator[](int);
        ColumnIterator& operator++();
        ColumnIterator operator++(int);
        ColumnIterator& operator--();
        ColumnIterator operator--(int);
        std::string return_name() {return name;}
    private:
        std::shared_ptr<Column> check(std::size_t, const std::string&) const;
        DataFrame& theDataFrame;
        std::weak_ptr<Column> wptr;
        std::size_t curr;
        std::deque<int> iteration_order;
        std::string name;

};
template <class T>
bool operator==(const DataFrame::ColumnIterator<T>& lhs,
                const DataFrame::ColumnIterator<T>& rhs) {
    return (lhs.name == rhs.name) && (lhs.curr == rhs.curr);
    //return lhs.curr == rhs.curr;
}

template <class T>
bool operator!=(const DataFrame::ColumnIterator<T>& lhs,
                const DataFrame::ColumnIterator<T>& rhs) {
    return !(lhs == rhs);
}

template <class T>
DataFrame::ColumnIterator<T> DataFrame::begin(const std::string& s) {
    try {
        make_unique_if(s);
        return ColumnIterator<T>(*this, s);
    } catch (std::out_of_range& e) {
        std::string m = "Column " + s + " not found, in\n:";
        throw std::out_of_range(m + __PRETTY_FUNCTION__);
    }
}

template <class T>
DataFrame::ColumnIterator<T> DataFrame::end(const std::string& s) {
    try {
        make_unique_if(s);
        return ColumnIterator<T>(*this, s, index.size());
    } catch (std::out_of_range& e) {
        std::string m = "Column " + s + " not found, in\n:";
        throw std::out_of_range(m + __PRETTY_FUNCTION__);
    }
}

template <class T>
std::shared_ptr<Column> DataFrame::ColumnIterator<T>::check(
        size_t i, const std::string& msg) const {
    auto ret = wptr.lock();
    if (!ret) throw std::runtime_error("Unbound ColumnIterator");
    if (i >- ret->size()) throw std::out_of_range(msg);
    return ret;
}

template <class T>
T& DataFrame::ColumnIterator<T>::operator*() const {
    auto p = check(curr, "dereferencing past end");
    int pos = iteration_order[curr];
    return (*p).template get_value<T>(pos);
}

template <class T>
T& DataFrame::ColumnIterator<T>::operator[](int i) {
    auto p = check(i, "dereferencing past end");
    int pos = iteration_order[i];
    return (*p).template get_value<T>(pos);
}

template <class T>
DataFrame::ColumnIterator<T>& DataFrame::ColumnIterator<T>::operator++() {
    check(curr, "increment past the end of ColumnIterator");
    ++curr;
    return *this;
}

template <class T>
DataFrame::ColumnIterator<T> DataFrame::ColumnIterator<T>::operator++(int) {
    DataFrame::ColumnIterator ret = *this;
    ++*this;
    return ret;
}

template <class T>
DataFrame::ColumnIterator<T>& DataFrame::ColumnIterator<T>::operator--() {
    --curr;
    check(curr, "decreement past the beginning of ColumnIterator");
    return *this;
}

template <class T>
DataFrame::ColumnIterator<T> DataFrame::ColumnIterator<T>::operator--(int) {
    DataFrame::ColumnIterator ret = *this;
    --*this;
    return ret;
}
template <typename T>
void DataFrame::fill_na(std::string s, T t) {
    iter<T> it = begin<T>(s);
    iter<T>  e = end<T>(s);
    std::transform(it, e, it, 
            [&t](auto& d) {return std::isnan(d) ? t : d;});
}

#endif
