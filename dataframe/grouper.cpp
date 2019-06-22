#include "grouper.h"
#include "ConstColumnIterator.h"
#include <set>
#include <numeric>
using std::string;
using std::vector;
using std::deque;
template <class T>
DataFrame::Grouper<T>::Grouper(DataFrame& a)
    : theDataFrame(a),
      old_index_names(a.index_names),
      index_positions(a.index_positions),
      column_names(a.column_names){}

template <typename T>
DataFrame::Grouper<T>::Grouper(DataFrame& a, const std::string& s)
    : theDataFrame(a) {
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
DataFrame DataFrame::Grouper<T>::summarize(
        double (*f)(const deque<int>&, const DataFrame::SharedCol&)) {
    DataFrame res;
    auto it = std::unique(index_positions.begin(), index_positions.end());
    index_positions.erase(it, index_positions.end());
    for (auto const n : column_names) {
        if (theDataFrame.columns[n.second]->type_name() == "double") {
            vector<double> tmp;
            SharedCol& col = theDataFrame.columns[n.second];
            for (const auto idx : index_positions)
                tmp.push_back(f(old_index_names[idx], col));
            res.append_column(n.first, std::make_shared<Column>(Column(tmp)));
        }
    }
    res.append_index(index_positions);
    res.assert_same_column_length(__PRETTY_FUNCTION__);
    return res;
}
template DataFrame 
DataFrame::Grouper<double>::summarize(double (*f)(const std::deque<int>&,
                                      const DataFrame::SharedCol&));
template DataFrame 
DataFrame::Grouper<string>::summarize(double (*f)(const std::deque<int>&,
                                      const DataFrame::SharedCol&));
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
