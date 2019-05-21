#include "Column.h"
#include <utility>

using std::string;
using std::vector;

Column::Column() {};

Column::Column(string n, vector<double>& dat) {
    name = n;
    data = dat;
}

Column::Column(string n, vector<double>&& dat) {
    name = n;
    data = std::move(dat);
}

