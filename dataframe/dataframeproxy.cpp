#include "dataframeproxy.h"
#include <vector>
#include <string>

using std::string;
using std::vector;


DataFrame::DataFrameProxy::DataFrameProxy(DataFrame& df,
                                          const vector<string>& i,
                                          const string& s)
    : theDataFrame(df), idxNames{i}, colNames{s} {};

DataFrame::DataFrameProxy::DataFrameProxy(DataFrame& df,
                                          const vector<string>& i,
                                          const vector<string>& s)
    : theDataFrame(df), idxNames{i}, colNames(s){};

