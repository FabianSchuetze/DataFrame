#include <algorithm>
#include <iostream>
#include <iterator>
#include "dataframe.h"
#include "dataframeproxy.h"
using std::string;
using std::vector;

vector<string> conversion(const string& title, const vector<int>& idx,
                          const Column& c) {
    vector<string> res = {title};
    int i = 0;
    for (auto const& x : idx) {
        if (i == 10) {
            res.push_back("---");
            break;
        }
        res.push_back(c.to_string(x));
        i++;
    }
    return res;
}

string::size_type width(vector<string>& inp) {
    string::size_type maxlen = 0;
    for (const auto& s : inp) maxlen = std::max(maxlen, s.size());
    return maxlen;
}

vector<string> frame(vector<string> vec) {
    vector<string> ret{"\n"};
    string::size_type maxlen = width(vec);
    string border(maxlen + 4, '-');
    border += "\n";
    ret.push_back(border);
    int i = 0;
    for (auto const& s : vec) {
        ret.push_back("| " + s + string(maxlen - s.size(), ' ') + " |\n");
        if (i == 0) ret.push_back(border);
        i++;
    }
    ret.push_back(border);
    return ret;
}

void hcat(vector<string>& lhs, const vector<string> rhs) {
    for (size_t i = 0; i < lhs.size(); ++i) {
        string tmp = lhs[i];
        lhs[i] = tmp.substr(0, tmp.size() - 1) + rhs[i].substr(1);
    }
}

vector<string> frame_index(vector<string> inp) {
    inp.insert(inp.begin(), string(""));
    vector<string>::iterator end;
    if (inp.size() > 11)
        end = inp.begin() + 12;
    else 
        end = inp.end();
    vector<string> output = frame(vector<string>(inp.begin(),end));
    return output;
}

vector<string> rhs_truncate(vector<string> inp) {
    inp.insert(inp.begin(), string(""));
    std::fill(inp.begin(), inp.end(), "---");
    vector<string>::iterator end;
    if (inp.size() > 11)
        end = inp.begin() + 12;
    else 
        end = inp.end();
    vector<string> output = frame(vector<string>(inp.begin(),end));
    return output;
}
void check_positions(vector<int>& inp) {
    for (const int& x : inp)
        if (x == -1) throw std::runtime_error("Could not find the index");
}

std::ostream& operator<<(std::ostream& os, const DataFrame& df) {
    vector<string> output = frame_index(df.get_index_names());
    vector<int> positions = df.get_index_positions(df.index_positions);
    check_positions(positions);
    for (const std::pair<string, int>& x : df.column_names) {
        vector<string> rhs =
            frame(conversion(x.first, positions, *df.columns[x.second]));
        hcat(output, rhs);
        if (output[1].size() > 60)  {
            hcat(output, rhs_truncate(rhs));
            break;
        }
    }
    std::copy(output.begin(), output.end(),
              std::ostream_iterator<string>(os, " "));
    return os;
}

std::ostream& operator<<(std::ostream& os,
                         const DataFrame::DataFrameProxy& df) {
    vector<string> output = frame_index(df.idxNames);
    vector<int> subset = df.theDataFrame.get_index_positions(df.idxNames);
    check_positions(subset);
    for (string const& colName : df.colNames) {
        std::shared_ptr<Column> c = df.theDataFrame.get_unique(colName, subset);
        vector<string> rhs = frame(conversion(colName, subset, *c));
        hcat(output, rhs);
        if (output[1].size() > 60)  {
            hcat(output, rhs_truncate(rhs));
            break;
        }
    }
    std::copy(output.begin(), output.end(),
              std::ostream_iterator<string>(os, " "));
    return os;
}
