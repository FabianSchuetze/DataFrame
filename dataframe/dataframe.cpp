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
    // I think that's not a good decision, I should leave and define the data
    // in a subfunction
    data.push_back(new vector<double> (2,0.));
    data.push_back(new vector<double> (2,0.));
}

DataFrame::DataFrame(const map<string, int>& rhs_index,
                     const map<int, string>& rhs_rev_index,
                     const map<string, int>& rhs_columns) {
    index=rhs_index;
    rev_index = rhs_rev_index;
    columns = rhs_columns;
    map<int, string> new_rev_col;
    new_rev_col[0] = "first";
    rev_columns = new_rev_col;
    data.push_back(new vector<double> (2,12.));
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
    int pos;
    map<string, int> new_index, new_col;
    map<int, string> new_rev_index;
    vector<double>* new_data;
    //try {
    pos = columns.at(s);
    std::cout << "abc";
    new_col[s] = 0;
    new_index = get_index();
    new_rev_index = get_rev_index();
    new_data = data[pos];
    //} catch(const std::domain_error) {
        //int pos = index.at(s);
        //new_col[s];
        //new_index = get_columns();
        //new_rev_index = get_rev_columns();
        //new_data->push_back(data[0]->at(pos));
        //new_data->push_back(data[1]->at(pos));
    //}
    DataFrame new_df(new_index, new_rev_index, new_col);
    //vector<double>* res = data[pos];
    new_df.data[0] = new_data;
    //new_df.data[0]  =res;
    return new_df;
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
