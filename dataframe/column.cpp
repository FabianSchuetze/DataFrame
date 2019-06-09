#include "column.h"
#include <cmath>
#include <iostream>
using std::holds_alternative;
using std::pair;
using std::string;
using std::transform;
using std::vector;

Column::Column(const Column& c, const vector<int>& subsets) {
    if (const vector<double>* val = std::get_if<vector<double>>(&c.col)) {
        col = vector<double>();
        for (int i : subsets) push_back<double>(val->at(i));
    } else if (const vector<string>* val =
                   std::get_if<vector<string>>(&c.col)) {
        col = vector<string>();
        for (int i : subsets) push_back<string>(val->at(i));
    }
}

void Column::replace_nan() {
    for (size_t i = 0; i < size(); i++) replace_nan(i);
}

string Column::type_name() {
    if (std::get_if<vector<double>>(&col))
        return "double";
    else if (std::get_if<vector<string>>(&col))
        return "string";
    else
        throw std::invalid_argument("Column has no elements");
}

void Column::push_back_nan() {
    typedef std::numeric_limits<double> nan;
    if (vector<double>* val = std::get_if<vector<double>>(&col))
        val->push_back(nan::quiet_NaN());
    else if (vector<string>* val = std::get_if<vector<string>>(&col))
        val->push_back("NA");
}

void Column::replace_nan(int i) {
    typedef std::numeric_limits<double> nan;
    if (vector<double>* val = std::get_if<vector<double>>(&col))
        val->at(i) = nan::quiet_NaN();
    else if (vector<string>* val = std::get_if<vector<string>>(&col))
        val->at(i) = "NA";
}

template <class T>
void Column::push_back(const T t) {
    std::get<vector<T>>(col).push_back(t);
}

const size_t Column::size() { return static_cast<const Column&>(*this).size(); }

const size_t Column::size() const {
    if (holds_alternative<vector<double>>(col))
        return std::get<vector<double>>(col).size();
    else if (holds_alternative<vector<string>>(col))
        return std::get<vector<string>>(col).size();
    else
        return 0;
}

std::string Column::to_string(int i) const {
    if (const vector<double>* val = std::get_if<vector<double>>(&col))
        return std::to_string(val->at(i));
    else if (const vector<string>* val = std::get_if<vector<string>>(&col))
        return val->at(i);
    else
        throw std::invalid_argument("Position i not in vector");
}

template <typename T>
bool val_is_null(const T& t) {
    if constexpr (std::is_same<T, double>::value)
        return std::isnan(t);
    else if (std::is_same<T, string>::value)
        return t == "NA";
}

template <typename T>
bool is_valid_pair(const vector<T>& lhs, const vector<T>& rhs,
                   const pair<int,int>& pair) {
    bool exists = pair.second > -1;
    bool lhs_is_not_nan = !val_is_null(lhs.at(pair.first));
    bool rhs_is_not_nan = !val_is_null(rhs.at(pair.first));
    return exists && lhs_is_not_nan && rhs_is_not_nan;
}

template <typename T>
void Column::add_elements(vector<T>* lhs, const vector<T>& rhs,
                          const vector<pair<int, int>>& indices) {
    for (auto const& x : indices) {
        if (is_valid_pair<T>(*lhs, rhs, x))
            lhs->at(x.first) += rhs[x.second];
        else
            replace_nan(x.first);
    }
}

bool Column::is_null(size_t pos) {
    if (pos >= size()) throw std::out_of_range("indexing Column past its size");
    if (const vector<double>* v = std::get_if<vector<double>>(&col))
        return std::isnan(v->at(pos));
    else if (const vector<string>* v = std::get_if<vector<string>>(&col))
        return v->at(pos) == "NA";
    return true;  // avoid compiler warning
}

Column& Column::plus(const Column& rhs, const vector<pair<int, int>>& indices) {
    if (vector<double>* vec = std::get_if<vector<double>>(&col)) {
        const vector<double>* other = std::get_if<vector<double>>(&rhs.col);
        add_elements(vec, *other, indices);
    } else if (vector<string>* vec = std::get_if<vector<string>>(&col)) {
        const vector<string>* other = std::get_if<vector<string>>(&rhs.col);
        add_elements(vec, *other, indices);
    }
    return *this;
}

template <typename T>
vector<T> transform_column(const vector<T>* v, const T& t) {
    vector<T> res = vector<T>(v->size(), t);
    transform(v->begin(), v->end(), res.begin(), res.begin(), std::plus<T>());
    return res;
}

Column operator+(const Column& c, double d) {
    if (const vector<double>* vec = std::get_if<vector<double>>(&c.col))
        return Column(transform_column(vec, d));
    else {
        string msg = ": Cant add double " + std::to_string(d) +
                     ": incompatible column types";
        throw std::invalid_argument(msg);
    }
}

Column operator+(const Column& c, const string& d) {
    if (const vector<string>* vec = std::get_if<vector<string>>(&c.col))
        return Column(transform_column(vec, d));
    else {
        string msg = "Cant add string " + d + ": incompatible column types";
        throw std::invalid_argument(msg);
    }
}
