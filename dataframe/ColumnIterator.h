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
        ColumnIterator();
        ColumnIterator(DataFrame& a, int n, size_t sz=0):
            theDataFrame(a), wptr(a.columns[n]), curr(sz) {}
        T& operator*() const;
        T& operator[](int);
        ColumnIterator& operator++();
        ColumnIterator operator++(int);
        ColumnIterator& operator--();
        ColumnIterator operator--(int);
    private:
        std::shared_ptr<Column> check(std::size_t, const std::string&) const;
        DataFrame& theDataFrame;
        std::weak_ptr<Column> wptr;
        std::size_t curr;

};
template <class T>
bool operator==(const DataFrame::ColumnIterator<T>& lhs,
                const DataFrame::ColumnIterator<T>& rhs) {
    return lhs.curr == rhs.curr;
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
        return ColumnIterator<T>(*this, column_names.at(s));
    } catch (std::out_of_range& e) {
        throw std::out_of_range("Column: " + s + " not found");
    }
}

template <class T>
DataFrame::ColumnIterator<T> DataFrame::end(const std::string& s) {
    try {
        make_unique_if(s);
        return ColumnIterator<T>(*this, column_names.at(s), index_names.size());
    } catch (std::out_of_range& e) {
        throw std::out_of_range("Column: " + s + " not found");
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
    std::string name = theDataFrame.index_positions[curr];
    // HERE CURR CHECKS HOW MUCH WE HAVE ALREADY ITERATED, WE WOULD NEED TO
    // KNOW WHAT! WE ALREADY ITERATED, AS IF THE ITERATOR HAS ITS OWN SEARCH
    int pos = theDataFrame.find_index_position(name);
    return (*p).template get_value<T>(pos);
}

template <class T>
T& DataFrame::ColumnIterator<T>::operator[](int i) {
    auto p = check(i, "dereferencing past end");
    std::string name = theDataFrame.index_positions[i];
    int pos = theDataFrame.find_index_position(name);
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
