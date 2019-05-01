#include "dataframe.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

using std::find;
using std::map;
using std::string;
using std::vector;

void DataFrame::print() {
    for (auto const& x : rev_index) {
        std::cout << x.second << "\t";
        for (auto const& y : rev_columns) {
            //std::cout << x.second << std::endl;
            //std::cout << y.second << std::endl;
            double val = (*this)(x.second, y.second);
            std::cout << val << "\t";
        }
        std::cout << "\n";
    }
}

DataFrame::DataFrame(const map<string, int>& rhs_index,
                     const map<int, string>& rhs_rev_index,
                     const map<string, int>& rhs_columns,
                     const map<int, string>& rhs_rev_columns) {
    index = rhs_index;
    rev_index = rhs_rev_index;
    columns = rhs_columns;
    rev_columns = rhs_rev_columns;
}

void DataFrame::init_map(const vector<string> container, map<string, int>& dict,
                         map<int, string>& rev_dict) {
    vector<string>::const_iterator iter = container.begin();
    int val = 0;
    for (; iter != container.end(); ++iter) {
        rev_dict[val] = *iter;
        dict[*iter] = val++;
    }
}

DataFrame::DataFrame(const vector<string>& idx) {
    init_map(idx, index, rev_index);
}

DataFrame::DataFrame(const vector<string>& idx, const vector<string>& cols,
                     const vector<data_col>& datainp) {
    // const won't work?!? What is going on?
    init_map(idx, index, rev_index);
    init_map(cols, columns, rev_columns);
    typedef vector<vector<double> >::size_type sz;
    for (sz i = 0; i != datainp.size(); ++i) {
        data.push_back(datainp[i]);
    }
}

DataFrame DataFrame::operator()(const std::string& s){
    int pos;
    map<string, int> new_index, new_col;
    map<int, string> new_rev_index, new_rev_col;
    vector<double>* new_data;
    vector<double> new_col_vec;
    try {
        pos = find_index(columns, s);
        new_col[s] = 0;
        new_rev_col[0] = s;
        new_index = get_index();
        new_rev_index = get_rev_index();
        new_data = data[pos]; 
    } catch (std::domain_error) {
        pos = find_index(index, s);
        new_col[s] = 0;
        new_rev_col[0] = s;
        new_index = get_columns();
        new_rev_index = get_rev_columns();
        std::cout << "abc\n";
        // I think this create a new object!!! Avoid copy!!!
        for (int i = 0; i < 2; ++i) {
            new_col_vec.push_back(data[i]->at(pos));
        }
        std::cout << "abc2\n";
        // I think this create a new object!!! Avoid copy!!!
        new_data = {&new_col_vec};
    }
    std::cout << new_data << '\n';
    DataFrame new_df(new_index, new_rev_index, new_col, new_rev_col);
    new_df.data = {&new_col_vec};
    std::cout << &new_df.data[0]->at(0) << "\n";
    std::cout << "end of function\n";
    return new_df;
}

int DataFrame::find_index(const map<string, int>& dict, const std::string& s) const {
    if (!(dict.count(s) > 0)) {
        throw std::domain_error("Cannot find " + s + " in index or columns");
    }
    //map<string, int>::const_iterator it = dict.find(s);
    return dict.find(s)->second;
}

// what does pandas return in this case?
double DataFrame::operator()(const std::string& time, const std::string& col) {
    int pos_row = find_index(index, time);
    int pos_col = find_index(columns, col);
    vector<double>* p = data[pos_col];
    return p->at(pos_row);
}

DataFrame operator+(const DataFrame& df1, const DataFrame& df2) {
    DataFrame new_df(df1.get_index(), df1.get_rev_index(), df1.get_columns(),
                     df1.get_rev_columns());
    std::transform(df1.data[0]->begin(), df1.data[0]->end(),
                   df2.data[0]->begin(), new_df.data[0]->begin(),
                   std::plus<double>());
    std::transform(df1.data[1]->begin(), df1.data[1]->end(),
                   df2.data[1]->begin(), new_df.data[1]->begin(),
                   std::plus<double>());
    return new_df;
}
