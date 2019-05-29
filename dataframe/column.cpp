#include "column.h"
using std::string;
using std::vector;
template <class T>
void Column::push_back(const T t) {
    std::get<vector<T>>(col).push_back(t);
}

vector<double>::iterator Column::begin() {
    if (std::holds_alternative<vector<double>>(col))
        return std::get<vector<double>>(col).begin();
    else 
        throw std::invalid_argument("No double");
}

vector<double>::iterator Column::end() {
    if (std::holds_alternative<vector<double>>(col))
        return std::get<vector<double>>(col).end();
    else 
        throw std::invalid_argument("No double");
}

int Column::size(){
    if (std::holds_alternative<dvec>(col))
        return std::get<dvec>(col).size();
    else if (std::holds_alternative<svec>(col))
        return std::get<svec>(col).size();
    else
        return 0;
}

//template <typename V> V Column::operator[](int pos) {
    //if (std::holds_alternative<vector<V>>(col))
        //return std::get<vector<V>>(col)[pos];
    //else
        //throw std::invalid_argument("not in here");
//}



std::ostream& operator<<(std::ostream& os, const Column& df) {
    string output;
    if (std::holds_alternative<vector<double>>(df.col)) {
        vector<double> tmp = std::get<vector<double>>(df.col);
        for (double& j : tmp)
            output += (std::to_string(j) + " ");
    } else if (std::holds_alternative<vector<std::string>>(df.col)) {
        vector<string> tmp = std::get<vector<string>>(df.col);
        for (string& j : tmp)
            output += (j + " ");
    }
    os << output;
    return os;
}
