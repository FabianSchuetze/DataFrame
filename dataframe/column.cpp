#include "column.h"
#include <algorithm>
using std::string;
using std::vector;
using std::plus;
using std::holds_alternative;
using std::transform;

template <typename V> V Column::operator[](int pos) {
    if (holds_alternative<vector<V>>(col))
        return std::get<vector<V>>(col)[pos];
    else
        throw std::invalid_argument("not in here");
}
template double Column::operator[](int);
template string Column::operator[](int);

void Column::replace_nan() {
    typedef std::numeric_limits<double> nan;
    if (holds_alternative<vector<double>>(col))
        col = vector<double>(size(), nan::quiet_NaN()); 
    else if (holds_alternative<vector<string>>(col))
        col = vector<string>(size(), "NOT_AVAILABLE");
}

template <class T>
void Column::push_back(const T t) {
    std::get<vector<T>>(col).push_back(t);
}

const int Column::size() {
    if (holds_alternative<vector<double>>(col))
        return std::get<vector<double>>(col).size();
    else if (holds_alternative<vector<string>>(col))
        return std::get<vector<string>>(col).size();
    else
        return 0;
}

void Column::append_string(string& s, int pos) {
    if (vector<double>* val = std::get_if<vector<double>>(&col))
        s += std::to_string(val->at(pos));
    if (vector<string>* val = std::get_if<vector<string>>(&col))
        s += val->at(pos);
}

std::ostream& operator<<(std::ostream& os, const Column& df) {
    if (const vector<double>* v = std::get_if<vector<double>>(&df.col))
        for (const double& j: *v) os << (std::to_string(j) + " " );
    else if (const vector<string>* v = std::get_if<vector<string>>(&df.col))
        for (const string& j: *v) os << (j + " " );
    return os;
}

Column& Column::operator+=(const Column& rhs) {
    if (vector<double>* vec = std::get_if<vector<double>>(&col)) {
        const vector<double>* other = std::get_if<vector<double>>(&rhs.col);
        transform(other->begin(), other->end(), vec->begin(),
                  vec->begin(), plus<double>());
    } else if (vector<string>* vec = std::get_if<vector<string>>(&col)) {
        const vector<string>* other = std::get_if<vector<string>>(&rhs.col);
        transform(other->begin(), other->end(), vec->begin(), vec->begin(), 
                  plus<string>());
    }
    return *this;
}

template <typename T> 
vector<T> transform_column(const vector<T>* v, const T& t) {
    vector<T> res = vector<T>(v->size(), t);
    transform(v->begin(), v->end(), res.begin(), res.begin(), plus<T>());
    return res;
}

Column operator+(const Column& c, double d) {
    if (const vector<double>* vec = std::get_if<vector<double>>(&c.col))
        return Column(transform_column(vec, d));
    else 
        throw std::invalid_argument("Cant combine a non-double with a double");
}

Column operator+(const Column& c, const string& d) {
    if (const vector<string>* vec = std::get_if<vector<string>>(&c.col))
        return Column(transform_column(vec, d));
    else 
        throw std::invalid_argument("Cant combine a non-double with a double");
}
