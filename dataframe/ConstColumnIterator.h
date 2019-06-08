#include "dataframe.h"
#include <iostream>

class Column;
template <class T>
class DataFrame::ConstColumnIterator {
    public:
        template <class V>
        friend bool operator==(const const_iter<V>&, const const_iter<V>&);
        template <class V>
        friend bool operator!=(const const_iter<V>&, const const_iter<V>&);
        ConstColumnIterator();
        ConstColumnIterator(const DataFrame& a, int n, size_t sz=0):
            theDataFrame(a), wptr(a.columns[n]), curr(sz) {}
        const T& operator*() const;
        ConstColumnIterator& operator++();
        ConstColumnIterator operator++(int);
        ConstColumnIterator& operator--();
        ConstColumnIterator operator--(int);
    private:
        std::shared_ptr<Column> check(std::size_t, const std::string&) const;
        const DataFrame& theDataFrame;
        std::weak_ptr<Column> wptr;
        std::size_t curr;

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
        return ConstColumnIterator<T>(*this, column_names.at(s), index_names.size());
    } catch (std::out_of_range& e) {
        throw std::out_of_range("Column: " + s + " not found");
    }
}

template <class T>
std::shared_ptr<Column> DataFrame::ConstColumnIterator<T>::check(
        size_t i, const std::string& msg) const {
    auto ret = wptr.lock();
    if (!ret) throw std::runtime_error("Unbound ConstColumnIterator");
    if (i >- ret->size()) throw std::out_of_range(msg);
    return ret;
}

template <class T>
const T& DataFrame::ConstColumnIterator<T>::operator*() const {
    auto p = check(curr, "dereferencing past end");
    int pos = theDataFrame.index_names[curr].second;
    return (*p).template get_value<T>(pos);
}

template <class T>
DataFrame::ConstColumnIterator<T>& DataFrame::ConstColumnIterator<T>::operator++() {
    check(curr, "increment past the end of ConstColumnIterator");
    ++curr;
    return *this;
}

template <class T>
DataFrame::ConstColumnIterator<T> DataFrame::ConstColumnIterator<T>::operator++(int) {
    DataFrame::ConstColumnIterator ret = *this;
    ++*this;
    return ret;
}

template <class T>
DataFrame::ConstColumnIterator<T>& DataFrame::ConstColumnIterator<T>::operator--() {
    --curr;
    check(curr, "decreement past the beginning of ConstColumnIterator");
    return *this;
}

template <class T>
DataFrame::ConstColumnIterator<T> DataFrame::ConstColumnIterator<T>::operator--(int) {
    DataFrame::ConstColumnIterator ret = *this;
    --*this;
    return ret;
}
