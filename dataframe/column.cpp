#include "column.h"
#include <algorithm>
using std::string;
using std::vector;

template <typename V> V Column::operator[](int pos) {
    if (std::holds_alternative<vector<V>>(col))
        return std::get<vector<V>>(col)[pos];
    else
        throw std::invalid_argument("not in here");
}
template double Column::operator[](int);
template string Column::operator[](int);

void Column::replace_nan() {
    typedef std::numeric_limits<double> nan;
    if (std::holds_alternative<vector<double>>(col))
        col = vector<double>(size(), nan::quiet_NaN()); 
    else if (std::holds_alternative<vector<string>>(col))
        col = vector<string>(size(), "NOT_AVAILABLE");
}

template <class T>
void Column::push_back(const T t) {
    std::get<vector<T>>(col).push_back(t);
}

int Column::size() {
    if (std::holds_alternative<vector<double>>(col))
        return std::get<vector<double>>(col).size();
    else if (std::holds_alternative<vector<string>>(col))
        return std::get<vector<string>>(col).size();
    else
        return 0;
}

void Column::append_string(std::string& s, int pos) {
    if (vector<double>* val = std::get_if<std::vector<double>>(&col))
        s += std::to_string(val->at(pos));
    if (vector<std::string>* val = std::get_if<std::vector<std::string>>(&col))
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
    if (vector<double>* dvec = std::get_if<vector<double>>(&col)) {
        const vector<double>* other = std::get_if<vector<double>>(&rhs.col);
        std::transform(other->begin(), other->end(), dvec->begin(),
                       dvec->begin(), std::plus<double>());
    } else if (vector<string>* svec = std::get_if<vector<string>>(&col)) {
        const vector<string>* other = std::get_if<vector<string>>(&rhs.col);
        std::transform(other->begin(), other->end(), svec->begin(),
                       svec->begin(), std::plus<string>());
    }
    return *this;
}

template <typename T> vector<T> transform_column(const vector<T>* rhs, T t) {
    vector<T> res = vector<T>(rhs->size(), t);
    std::transform(rhs->begin(), rhs->end(), res.begin(), res.begin(),
                   std::plus<T>());
    return res;
}

Column operator+(const Column& c, double d) {
    if (const vector<double>* dvec = std::get_if<vector<double>>(&c.col))
        return Column(transform_column(dvec, d));
    else 
        throw std::invalid_argument("Cant combine a non-double with a double");
}

Column operator+(const Column& c, string d) {
    if (const vector<string>* dvec = std::get_if<vector<string>>(&c.col))
        return Column(transform_column(dvec, d));
    else 
        throw std::invalid_argument("Cant combine a non-double with a double");
}
