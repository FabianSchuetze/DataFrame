#include "dataframe.h"
#include "dataframeproxy.h"
#include <algorithm>
#include <iostream>
using std::string;
using std::vector;

vector<string> conversion(const string& title, const Column& c) {
    vector<string> res = {title};
    for (int i = 0; i < c.size() && i < 5; ++i) {
        res.push_back(c.to_string(i));
    }
    return res;
}

string::size_type width(vector<string>& inp) {
    string::size_type maxlen = 0;
    for (const auto& s: inp)
        maxlen = std::max(maxlen, s.size());
    return maxlen;
}

vector<string> frame(vector<string> vec) {
    vector<string> ret{"\n"};
    string::size_type maxlen = width(vec);
    string border(maxlen + 4, '-');
    border += "\n";
    ret.push_back(border);
    int i =0;
    for (auto const& s : vec) {
        ret.push_back("| " + s + string(maxlen - s.size(), ' ') + " |\n");
        if (i == 0) ret.push_back(border);
        i++;
    }
    ret.push_back(border);
    return ret;
}

void hcat(vector<string>& lhs, const vector<string> rhs) {
    for (int i = 0; i < lhs.size(); ++i) {
        string tmp = lhs[i];
        lhs[i] = tmp.substr(0, tmp.size() -1) + rhs[i].substr(1);
    }
}

std::ostream& operator<<(std::ostream& os, const DataFrame& df) {
    vector<string> titles = df.get_column_names();
    vector<string> index = df.get_index_names();
    index.insert(index.begin(), "index");
    vector<string> output = frame(index);
    for (int i = 0; i < titles.size(); ++i) {
        vector<string> rhs = frame(conversion(titles[i], *df.columns[i]));
        hcat(output, rhs);
    }
    os << output;
    return os;
}

std::ostream& operator<<(std::ostream& os,
                         const DataFrame::DataFrameProxy& df) {
    vector<string> titles = df.colNames;
    vector<string> index = df.idxNames;
    index.insert(index.begin(), "index");
    vector<string> output = frame(index);
    vector<int> subset = df.theDataFrame.get_index_positions(df.idxNames);
    for (auto const& title : titles) {
        std::shared_ptr<Column> col = df.theDataFrame.get_unique(title, subset);
        vector<string> rhs = frame(conversion(title, *col));
        hcat(output, rhs);
    }
    os << output;
    return os;
}
