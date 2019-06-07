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

vector<string> frame_index(vector<string> inp) {
    inp.insert(inp.begin(), string(""));
    vector<string> output = frame(inp);
    return output;
}
std::ostream& operator<<(std::ostream& os, const DataFrame& df) {
    vector<string> output = frame_index(df.get_index_names());
    for (const std::pair<string, int>& x : df.column_names) {
        vector<string> rhs = frame(conversion(x.first, *df.columns[x.second]));
        hcat(output, rhs);
    }
    os << output;
    return os;
}

std::ostream& operator<<(std::ostream& os,
                         const DataFrame::DataFrameProxy& df) {
    vector<string> output = frame_index(df.idxNames);
    vector<int> subset = df.theDataFrame.get_index_positions(df.idxNames);
    for (string const& colName : df.colNames) {
        std::shared_ptr<Column> c = df.theDataFrame.get_unique(colName, subset);
        vector<string> rhs = frame(conversion(colName, *c));
        hcat(output, rhs);
    }
    os << output;
    return os;
}
