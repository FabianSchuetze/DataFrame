#include "grouper.h"
#include "GroupbyFunctions.h"
#include <numeric>
#include <set>
#include "ConstColumnIterator.h"
using std::deque;
using std::string;
using std::vector;
template <class T>
DataFrame::Grouper<T>::Grouper(DataFrame& a)
    : columns(a.columns),
      old_index_names(a.index_names),
      index_positions(a.index_positions),
      column_names(a.column_names) {}

template <typename T>
DataFrame::Grouper<T>::Grouper(DataFrame& a, const std::string& s)
    : columns(a.columns) {
    std::deque<int> old_index_positions = a.find_index_position();
    int i = 0;
    for (const_iter<T> b = a.cbegin<T>(s); b != a.cend<T>(s); ++b) {
        index_positions.push_back(b.to_string());
        old_index_names[b.to_string()].push_back(old_index_positions[i++]);
    }
    column_names = a.column_names;
    column_names.erase(s);
}

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
    res.append_index(index_positions);
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

template <class T>
DataFrame::Grouper<T> DataFrame::groupby(const string& s) {
    sort_by_column_template<T>(s);
    Grouper<T> grouper(*this, s);
    return grouper;
}
template DataFrame::Grouper<string> DataFrame::groupby();
template DataFrame::Grouper<string> DataFrame::groupby(const string&);
template DataFrame::Grouper<double> DataFrame::groupby(const string&);

template <class T>
DataFrame::Grouper<T> DataFrame::DataFrameProxy::groupby(const string& s) {
    return DataFrame(*this).groupby<T>(s);
}
template DataFrame::Grouper<string> DataFrame::DataFrameProxy::groupby(
    const string&);
template DataFrame::Grouper<double> DataFrame::DataFrameProxy::groupby(
    const string&);
