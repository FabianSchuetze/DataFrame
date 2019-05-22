#include "Column.h"
#include "dataframe.h"

using std::ostream;
using std::string;

ostream& operator<<(ostream& os, const DataFrame& df) {
    // first line
    string output;
    double num;
    for (auto const& x : df.column_names) output += x.first + ' ';
    output += '\n';
    for (int i = 0; i < df.columns[0]->size(); ++i) {
        for (int j = 0; j < df.size(); ++j) {
            num = (*df.columns[j])[i];
            output += std::to_string(num) + ' ';
        }
        output += '\n';
    }
    output += '\n';
    os << output;
    return os;
}
