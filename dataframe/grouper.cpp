#include "grouper.h"
#include "GroupbyFunctions.h"
#include <numeric>
#include <set>
#include "ConstColumnIterator.h"
using std::deque;
using std::string;
using std::vector;

template <class T>
vector<string> DataFrame::Grouper<T>::elegible_types(const string& s) {
    vector<string> elegible;
    if (s == "string")
        for (auto const n : column_names) elegible.push_back(n.first);
    else {
        for (auto const n : column_names) {
            if (columns[n.second]->type_name() == s)
                elegible.push_back(n.first);
        }
    }
    return elegible;
}

template <class T>
void DataFrame::Grouper<T>::make_index_unique() {
    auto it = std::unique(index_positions.begin(), index_positions.end());
    index_positions.erase(it, index_positions.end());
}
template <class T>
DataFrame DataFrame::Grouper<T>::summarize(Statistic* f) {
    DataFrame res;
    make_index_unique();
    for (const string& s : elegible_types(f->get_name())) {
        vector<double> tmp;
        SharedCol& col = columns[column_names[s]];
        for (const auto idx : index_positions)
            tmp.push_back(f->func(old_index_names[idx], col));
        res.append_column(s, std::make_shared<Column>(Column(tmp)));
    }
    res.index.append_index(index_positions);
    res.assert_same_column_length(__PRETTY_FUNCTION__);
    return res;
}
template DataFrame DataFrame::Grouper<double>::summarize(Statistic*);
template DataFrame DataFrame::Grouper<string>::summarize(Statistic*);

template <class T>
DataFrame::Grouper<T> DataFrame::groupby() {
    sort_by_index();
    Grouper<T> grouper(*this);
    return grouper;
}
