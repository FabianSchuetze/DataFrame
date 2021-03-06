#include "column.h"
#include <cmath>
#include <ios>
#include <iostream>
#include <memory>
#include <sstream>
using std::deque;
using std::get_if;
using std::holds_alternative;
using std::is_same;
using std::pair;
using std::string;
using std::transform;
using std::vector;

template <typename T>
void Column::copy_vector(const vector<T>* val, const deque<int>& subsets) {
    col = vector<T>();
    for (int i : subsets) push_back<T>(val->at(i));
}

Column::Column(const Column& c, const deque<int>& subsets) {
    if (const vector<double>* val = get_if<vector<double>>(&c.col))
        copy_vector<double>(val, subsets);
    else if (const vector<string>* val = get_if<vector<string>>(&c.col))
        copy_vector<string>(val, subsets);
    else if (const vector<bool>* val = get_if<vector<bool>>(&c.col))
        copy_vector<bool>(val, subsets);
}

void Column::replace_nan() {
    for (size_t i = 0; i < size(); i++) replace_nan(i);
}

string Column::type_name() {
    if (std::get_if<vector<double>>(&col))
        return "double";
    else if (std::get_if<vector<string>>(&col))
        return "string";
    else if (std::get_if<vector<bool>>(&col))
        return "bool";
    else {
        string m = string("Colum has no elements:\n") + __PRETTY_FUNCTION__;
        throw std::invalid_argument(m);
    }
}

void Column::push_back_nan() {
    typedef std::numeric_limits<double> nan;
    if (vector<double>* val = std::get_if<vector<double>>(&col))
        val->push_back(nan::quiet_NaN());
    else if (vector<string>* val = std::get_if<vector<string>>(&col))
        val->push_back("NA");
    else if (std::get_if<vector<bool>>(&col)) {
        string msg = "Attemting to push_back NA for a bool type causes failure";
        throw std::runtime_error(msg);
    }
}

void Column::replace_nan(int i) {
    typedef std::numeric_limits<double> nan;
    if (vector<double>* val = std::get_if<vector<double>>(&col))
        val->at(i) = nan::quiet_NaN();
    else if (vector<string>* val = std::get_if<vector<string>>(&col))
        val->at(i) = "NA";
    else if (std::get_if<vector<bool>>(&col)) {
        string msg = "Attemting to replace bool by NA causes failure";
        throw std::runtime_error(msg);
    }
}

bool potential_na(const std::string& s) {
    return (s.empty()) || (s == "None") || (s == "NaN") || (s == "null");
}
void Column::convert_and_push_back(const std::string& s) {
    if (potential_na(s)) {
        push_back_nan();
        return;
    }
    if (std::get_if<vector<double>>(&col)) try {
            push_back(std::stod(s));
        } catch (const std::invalid_argument& e) {
            std::string m("Attempt to convert: " + s + " into double, in:\n");
            throw std::invalid_argument(m + __PRETTY_FUNCTION__);
        }
    if (std::get_if<vector<string>>(&col)) push_back(s);
    if (std::get_if<vector<bool>>(&col)) {
        std::istringstream is(s);
        bool b;
        is >> std::boolalpha >> b;
        push_back(b);
    }
}

size_t Column::size() { return static_cast<const Column&>(*this).size(); }

size_t Column::size() const {
    if (holds_alternative<vector<double>>(col))
        return std::get<vector<double>>(col).size();
    else if (holds_alternative<vector<string>>(col))
        return std::get<vector<string>>(col).size();
    else if (holds_alternative<vector<bool>>(col))
        return std::get<vector<bool>>(col).size();
    else
        return 0;
}

void Column::copy_row(int pos) {
    if (const vector<double>* val = std::get_if<vector<double>>(&col))
        push_back<double>(val->at(pos));
    else if (const vector<string>* val = std::get_if<vector<string>>(&col))
        push_back<string>(val->at(pos));
    else if (const vector<bool>* val = std::get_if<vector<bool>>(&col))
        push_back<bool>(val->at(pos));
    else
        throw std::invalid_argument("Position i not in vector");
}

std::string Column::to_string(int i) const {
    if (const vector<double>* val = std::get_if<vector<double>>(&col))
        return std::to_string(val->at(i));
    else if (const vector<string>* val = std::get_if<vector<string>>(&col))
        return val->at(i);
    else if (const vector<bool>* val = std::get_if<vector<bool>>(&col))
        return std::to_string(val->at(i));
    else
        throw std::invalid_argument("Position i not in vector");
}

template <typename T>
bool vector_element_is_null(const T& t) {
    if constexpr (std::is_same<T, double>::value)
        return std::isnan(t);
    else if constexpr (std::is_same<T, string>::value)
        return t == "NA";
    else if constexpr (std::is_same<T, bool>::value)
        return false;
}

template <typename T>
bool is_valid_pair(const vector<T>& lhs, const vector<T>& rhs,
                   const pair<int, int>& pair) {
    if (pair.second < 0) return false;
    try {
        bool lhs_is_not_nan = !vector_element_is_null(lhs.at(pair.first));
        bool rhs_is_not_nan = !vector_element_is_null(rhs.at(pair.second));
        return lhs_is_not_nan && rhs_is_not_nan;
    } catch (std::out_of_range& e) {
        throw std::out_of_range("Couldnt get vector ele in 'is_valid_pair");
    }
}

template <typename T>
void Column::add_elements(vector<T>* lhs, const vector<T>& rhs,
                          const deque<pair<int, int>>& indices) {
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
    else if (std::get_if<vector<bool>>(&col))
        return false;
    else
        throw std::runtime_error("Cannot find value");
}

void Column::is_null(vector<int>& tmp) {
    if (vector<double>* v = std::get_if<vector<double>>(&col))
        std::transform(v->begin(), v->end(), tmp.begin(), tmp.begin(),
                       [](double& a, int& b) { return std::isnan(a) + b; });
    else if (const vector<string>* v = std::get_if<vector<string>>(&col))
        std::transform(v->begin(), v->end(), tmp.begin(), tmp.begin(),
                       [](const string& a, int& b) { return (a == "NA") + b; });
    else if (std::get_if<vector<bool>>(&col))
        ;
    else
        throw std::runtime_error("Cannot find value");
}

void Column::convert_bool_to_double() {
    if (vector<bool>* vec = std::get_if<vector<bool>>(&col)) {
        vector<double> tmp(vec->begin(), vec->end());
        col = tmp;
    } else
        throw std::invalid_argument("Can only convert boolean");
}

Column Column::convert_bool_to_double(const Column& rhs) {
    Column new_col = rhs;
    new_col.convert_bool_to_double();
    return new_col;
}

void Column::add_to_double(const Column& rhs,
                           const deque<pair<int, int>>& idx) {
    vector<double>* vec = std::get_if<vector<double>>(&col);
    if (const vector<double>* other = std::get_if<vector<double>>(&rhs.col))
        add_elements(vec, *other, idx);
    else if (std::get_if<vector<bool>>(&rhs.col))
        this->plus(convert_bool_to_double(rhs), idx);
    else
        throw std::invalid_argument("Rhs has the wrong type");
}

void Column::add_to_string(const Column& rhs,
                           const deque<pair<int, int>>& idx) {
    vector<string>* vec = std::get_if<vector<string>>(&col);
    if (const vector<string>* other = std::get_if<vector<string>>(&rhs.col))
        add_elements(vec, *other, idx);
    else
        throw std::invalid_argument("Can only add strings to strings");
}

Column& Column::plus(const Column& rhs, const deque<pair<int, int>>& indices) {
    if (std::get_if<vector<double>>(&col))
        add_to_double(rhs, indices);
    else if (std::get_if<vector<string>>(&col))
        add_to_string(rhs, indices);
    else if (std::get_if<vector<bool>>(&col)) {
        convert_bool_to_double();
        this->plus(rhs, indices);
    } else
        throw std::runtime_error("Cannot find the right addition");
    return *this;
}
