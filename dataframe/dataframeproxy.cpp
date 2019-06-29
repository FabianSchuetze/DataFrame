#include "dataframeproxy.h"

using std::string;
using std::vector;

//TEMPLATE FUNCTION MISSING
vector<std::deque<Index::ele>> create_index(vector<string> inp) {
    vector<std::deque<Index::ele>> res;
    for (string i: inp) {
        //std::deque<Index::ele>tmp{i};
        res.push_back(std::deque<Index::ele>{i});
    }
    return res;
}
DataFrame::DataFrameProxy::DataFrameProxy(DataFrame& df,
                                          const vector<string>& i,
                                          const string& s)
    : theDataFrame(df), colNames{s} { idxElements = create_index(i);}

DataFrame::DataFrameProxy::DataFrameProxy(DataFrame& df,
                                          const vector<string>& i,
                                          const vector<string>& s)
    : theDataFrame(df), colNames(s){idxElements = create_index(i);}
