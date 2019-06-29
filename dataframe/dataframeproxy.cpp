#include "dataframeproxy.h"

using std::string;
using std::vector;

DataFrame::DataFrameProxy::DataFrameProxy(DataFrame& df, const string& s)
    : theDataFrame(df), colNames{s} {}


DataFrame::DataFrameProxy::DataFrameProxy(DataFrame& df, const vector<string>& s)
    : theDataFrame(df), colNames(s) { }
