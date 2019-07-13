#ifndef GUARD_Grouper_h
#define GUARD_Grouper_h
#include <algorithm>
#include <functional>
#include <numeric>
#include "GroupbyFunctions.h"
#include "dataframe.h"
#include "dataframeproxy.h"

template <class... T>
class DataFrame::Grouper {
   public:
    Grouper(DataFrame&);
    Grouper(DataFrame&, DataFrame::const_iterator<T>&...);
    DataFrame summarize(Statistic*);
    DataFrame summarize(Statistic*, const std::initializer_list<std::string>&);
    DataFrame calculate_statistic(Statistic*, const std::vector<std::string>&);

   private:
    std::vector<std::string> elegible_types(const std::string&);
    std::vector<SharedCol> columns;
    Index group_index;
    std::map<std::string, int> group_column_names;
    void make_index_unique();
};

template <class... T>
DataFrame::Grouper<T...>::Grouper(DataFrame& a)
    : columns(a.columns), group_index(), group_column_names(a.column_names) {
    group_index.index_names = a.index.index_names;
    group_index.index_positions = a.index.index_positions;
}

template <typename T>
std::vector<T> return_vector(std::string n, DataFrame& df) {
    return std::vector<T>(df.cbegin<T>(n), df.cend<T>(n));
}

template <typename T1, typename... T>
void erase_column_name(std::map<std::string, int>& cols,
                       DataFrame::const_iterator<T1>& b1,
                       DataFrame::const_iterator<T>&... b) {
    cols.erase(b1.return_name());
    if constexpr (sizeof...(T) > 0) erase_column_name<T...>(cols, b...);
}

template <typename... T>
DataFrame::Grouper<T...>::Grouper(DataFrame& a,
                                  DataFrame::const_iterator<T>&... b)
    : columns(a.columns) {
    std::deque<int> old_pos = a.index.find_index_position();
    group_index = Index(old_pos, return_vector<T>(b.return_name(), a)...);
    group_column_names = a.column_names;
    erase_column_name<T...>(group_column_names, b...);
}

template <typename... T>
DataFrame::Grouper<T...> DataFrame::groupby(
    DataFrame::const_iterator<T>... it) {
    DataFrame df_copy = *this;
    df_copy.sort_by_column<T...>(it...);
    Grouper<T...> grouper(df_copy, it...);
    return grouper;
}
template <class... T>
std::vector<std::string> DataFrame::Grouper<T...>::elegible_types(
    const std::string& s) {
    std::vector<std::string> elegible;
    if (s == "string")
        for (auto const n : group_column_names) elegible.push_back(n.first);
    else {
        for (auto const n : group_column_names) {
            if (columns[n.second]->type_name() == s)
                elegible.push_back(n.first);
        }
    }
    return elegible;
}

template <class... T>
void DataFrame::Grouper<T...>::make_index_unique() {
    auto b = group_index.index_positions.begin();
    auto e = group_index.index_positions.end();
    auto it = std::unique(b, e);
    group_index.index_positions.erase(it, group_index.index_positions.end());
}

template <class... T>
DataFrame DataFrame::Grouper<T...>::calculate_statistic(
    Statistic* f, const std::vector<std::string>& names) {
    DataFrame res;
    make_index_unique();
    for (const std::string& s : names) {
        std::vector<double> tmp;
        SharedCol& col = columns[group_column_names[s]];
        for (const auto idx : group_index.index_positions)
            tmp.push_back(f->func(group_index.index_names[idx], col));
        res.append_column(s, std::make_shared<Column>(Column(tmp)));
    }
    res.index.append_index(group_index.index_positions);
    res.assert_same_column_length(__PRETTY_FUNCTION__);
    return res;
}

template <class... T>
DataFrame DataFrame::Grouper<T...>::summarize(Statistic* f) {
    return calculate_statistic(f, elegible_types(f->get_name()));
}

template <class... T>
DataFrame DataFrame::Grouper<T...>::summarize(
    Statistic* f, const std::initializer_list<std::string>& names) {
    std::vector<std::string> _names(names.begin(), names.end());
    return calculate_statistic(f, _names);
}
#endif
