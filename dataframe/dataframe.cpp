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
    for (auto const& x: rev_index) {
        std::cout << x.second << "\t";
        for (auto const& y: rev_columns) {
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
    index=rhs_index;
    rev_index = rhs_rev_index;
    columns = rhs_columns;
    rev_columns = rhs_rev_columns;
    data.push_back(new vector<double> (2,0.));
    data.push_back(new vector<double> (2,0.));
}

void DataFrame::init_map(const vector<string> container,
                         map<string, int>& dict,
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
                     vector<vector<double> >& datainp) {
    // const won't work?!? What is going on?
    init_map(idx, index, rev_index);
    init_map(cols, columns, rev_columns);
    typedef vector<vector<double> >::size_type sz;
    for (sz i = 0; i != datainp.size(); ++i) {
        data.push_back(&datainp[i]);
    }
}

// suppose first: I copy everything, not good but I do it
DataFrame DataFrame::operator()(const std::string& s) const {
    // very intersting: I only look for index, but why not columns?
    int pos = 0;
    vector<string> new_columns;
    for (auto const& y: rev_columns) new_columns.push_back(y.second);
    vector<string>new_rows = {"abvc"};
    vector<vector<double>*>::const_iterator it = data.begin();
    vector<vector<double>> results;
    vector<double> results_short;
    for (; it != data.end(); ++it) {
        double tmp = (*it)->at(pos);
        results_short.push_back(tmp);
    }
    results.push_back(results_short);
    return DataFrame(new_rows, new_columns, results);
}

int DataFrame::find_index(const map<string, int>& dict, const std::string& s) {
    if (!(dict.count(s) > 0)) {
        throw std::domain_error("Index s is not present");
    }
    return dict.at(s);
}

 double DataFrame::operator()(const std::string& time, const std::string& col)
{
    int pos_row = find_index(index, time);
    int pos_col = find_index(columns, col);
    vector<double>* p = data[pos_col];
    return p->at(pos_row);
}

DataFrame operator+(const DataFrame& df1, const DataFrame& df2) {
    DataFrame new_df(df1.get_index(), df1.get_rev_index(),
                     df1.get_columns(), df1.get_rev_columns());
    std::transform(df1.data[0]->begin(), df1.data[0]->end(), 
                   df2.data[0]->begin(), new_df.data[0]->begin(), std::plus<double>());
    std::transform(df1.data[1]->begin(), df1.data[1]->end(), 
                   df2.data[1]->begin(), new_df.data[1]->begin(), std::plus<double>());
    return new_df;
}
