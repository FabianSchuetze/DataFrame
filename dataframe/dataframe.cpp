#include "dataframe.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>

using std::find;
using std::map;
using std::string;
using std::vector;

void DataFrame::print() {
    for (auto const& x : rev_index) {
        std::cout << x.second << "\t";
        for (auto const& y : rev_columns) {
            double* p = (data[y.first] + x.first);
            std::cout << *p << "\t";
        }
        std::cout << "\n";
    }
}
void DataFrame::print() const {
    for (auto const& x : rev_index) {
        std::cout << x.second << "\t";
        for (auto const& y : rev_columns) {
            double* p = (data[y.first] + x.first);
            std::cout << *p << "\t";
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

void DataFrame::init_map(const vector<string>& container,
                         map<string, int>& dict, map<int, string>& rev_dict) {
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
                     vector<vector<double>>& datainp) {
    init_map(idx, index, rev_index);
    init_map(cols, columns, rev_columns);
    typedef vector<vector<double>>::size_type sz;
    for (sz i = 0; i != datainp.size(); ++i) {
        data.push_back(datainp[i].data());
    }
}

vector<int> DataFrame::find_subset(map<string, int>& new_idx,
                                   map<int, string>& new_rev_idx,
                                   const vector<string>& names,
                                   const map<string, int>& idx) const {
    vector<int> positions;
    for (const string& name : names) {
        if (!(idx.count(name) > 0)) {
            throw std::domain_error("Cannot find " + name + " in idx or cols");
        }
        int pos = idx.find(name)->second;
        positions.push_back(pos);
        new_idx[name] = pos;
        new_rev_idx[pos] = name;
    }
    return positions;
}

// VERY INTERESTING: CAN I HAVE THE NON-CONSTANT SLICE OPERATOR CREATE A NEW
// COLUMN IF IT WAS NOT ALREADY THERE? SEE THE LAST ANSWER HERE: 
// https://stackoverflow.com/questions/251159/what-is-the-use-of-const-overloading-in-c
// 
// LOOK AT THE MAP OPERATOR!!!!!!!!!!! INSERT VS SLICE!!!!
DataFrame& DataFrame::operator() (const vector<string>& s) const {
    map<string, int> new_index, new_col;
    map<int, string> new_rev_index, new_rev_col;
    vector<double*> new_data;
    try {  // check wheter user enters columns in s
        find_index(columns, s[0]); //needed?; already test in find_subset?
        vector<int> positions = find_subset(new_col, new_rev_col, s, columns);
        new_index = get_index();
        new_rev_index = get_rev_index();
    } catch (std::domain_error) {  // check if user entered rows in s
        find_index(index, s[0]);
        vector<int> pos = find_subset(new_index, new_rev_index, s, index);
        new_col = get_columns();
        new_rev_col = get_rev_columns();
    }
    DataFrame* new_df = new DataFrame(new_index, new_rev_index, new_col, new_rev_col);
    new_df->data = data;
    return *new_df;
}

DataFrame& DataFrame::operator() (const vector<string>& s){
    map<string, int> new_index, new_col;
    map<int, string> new_rev_index, new_rev_col;
    vector<double*> new_data;
    try {  // check wheter user enters columns in s
        find_index(columns, s[0]); //needed?; already test in find_subset?
        vector<int> positions = find_subset(new_col, new_rev_col, s, columns);
        new_index = get_index();
        new_rev_index = get_rev_index();
    } catch (std::domain_error) {  // check if user entered rows in s
        find_index(index, s[0]);
        vector<int> pos = find_subset(new_index, new_rev_index, s, index);
        new_col = get_columns();
        new_rev_col = get_rev_columns();
    }
    this->rev_index = new_rev_index;
    this->columns = new_col;
    this->rev_columns = new_rev_col;
    this->data = data;
    return *this;
}

int DataFrame::find_index(const map<string, int>& dict,
                          const std::string& s) const {
    if (!(dict.count(s) > 0)) {
        throw std::domain_error("Cannot find " + s + " in index or columns");
    }
    return dict.find(s)->second;
}
DataFrame& DataFrame::operator=(const DataFrame& rhs) {
    //DataFrame new_df new DataFrame();
    if (&rhs != this) {
        std::cout << "inside" << std::endl;
        //delete &data;
        //delete &index;
        data = rhs.data;
        std::cout << data[0] << std::endl;
        std::cout << rhs.data[0] << std::endl;
        index = rhs.index;
        rev_index = rhs.rev_index;
        columns = rhs.columns;
        rev_columns = rhs.rev_columns;
        std::cout << columns.begin()->first << std::endl;
        std::cout << columns.size() << std::endl;
    }
    std::cout << "completed" << "\n";
    return *this;
}

// can I combine this with the function above?
// DataFrame DataFrame::operator()(const std::string& time,
// const std::string& col) {
// int pos_index = find_index(index, time);
// int pos_col = find_index(columns, col);
// map<string, int> new_index, new_col;
// map<int, string> new_rev_index, new_rev_col;
// new_index[time] = 0, new_col[col] = 0;
// new_rev_index[0] = time, new_rev_col[0] = col;
// DataFrame new_df =
// DataFrame(new_index, new_rev_index, new_col, new_rev_col);
// vector<double*> new_data = {data[pos_col][pos_index]};
// new_df.data = {new_data};
// return new_df;
//}

// DataFrame operator+(const DataFrame& df1, const DataFrame& df2) {
// DataFrame new_df(df1.get_index(), df1.get_rev_index(), df1.get_columns(),
// df1.get_rev_columns());
// std::transform(df1.data[0].begin(), df1.data[0].end(),
// df2.data[0].begin(), new_df.data[0].begin(),
// std::plus<double*>());
// std::transform(df1.data[1].begin(), df1.data[1].end(),
// df2.data[1].begin(), new_df.data[1].begin(),
// std::plus<double*>());
// return new_df;
//}
