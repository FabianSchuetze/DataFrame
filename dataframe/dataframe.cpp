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

// I think I don't need the default constructor, do I?  DataFrame();

void DataFrame::init_map(const vector<string> container,
                         map<string, int>& dict) {
    vector<string>::const_iterator iter = container.begin();
    int val = 0;
    for (; iter != container.end(); ++iter) {
        dict[*iter] = val++;
    }
}

DataFrame::DataFrame(const vector<string>& idx) { init_map(idx, index); }

DataFrame::DataFrame(const vector<string>& idx, const vector<string>& cols,
                     vector<vector<double> >& datainp) {
    // const won't work?!? What is going on?
    init_map(idx, index);
    init_map(cols, columns);
    typedef vector<vector<double> >::size_type sz;
    for (sz i = 0; i != datainp.size(); ++i) {
        data.push_back(&datainp[i]);
    }
}

const std::vector<double> DataFrame::operator()(const std::string& s) {
    int pos = find_index(index, s);
    vector<vector<double>*>::const_iterator it = data.begin();
    vector<double> results;
    for (; it != data.end(); ++it) {
        results.push_back((*it)->at(pos));
    }
    return results;
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
