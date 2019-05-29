#include "Column.h"
#include <utility>

using std::string;
using std::vector;

Column::Column(){};

Column::Column(const string n, const vector<double>& dat) {
    name = n;
    data = dat;
}

Column::Column(const string n, vector<double>&& dat) {
    name = n;
    data = std::move(dat);
}

