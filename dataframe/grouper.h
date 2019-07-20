#ifndef GUARD_Grouper_h
#define GUARD_Grouper_h
#include <cxxabi.h>
#include <algorithm>
#include <functional>
#include <numeric>
#include "GroupbyFunctions.h"
#include "dataframe.h"
#include "dataframeproxy.h"

template <class... T>
class DataFrame::Grouper {
   public:
    // Grouper(DataFrame&);
    Grouper(DataFrame&, DataFrame::const_iterator<T>&...);
    /**
     * @brief Returns a dataframe with the calculated summary statistics.
     */
    DataFrame summarize(Statistic*);
    DataFrame summarize(Statistic*, const std::vector<std::string>&);

   private:
    DataFrame calc(Statistic*, const std::vector<std::string>&);
    std::vector<SharedCol> columns;
    Index group_index;
    std::map<std::string, int> group_column_names;
    void make_index_unique();
};

// FUNCTION NOT NEEDED AT THE MOMENT, MAYBE I CAN USE THIS TO GROUP BY INDEX?
// WHEN CLASS T = INDEX??? //

// template <class... T>
// DataFrame::Grouper<T...>::Grouper(DataFrame& a)
//: columns(a.columns), group_index(), group_column_names(a.column_names) {
// group_index.index_names = a.index.index_names;
// group_index.index_positions = a.index.index_positions;
//}

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

std::vector<std::string> elegible_types(
    const std::map<std::string, int>& names, const std::string& s,
    const std::vector<DataFrame::SharedCol>& cols) {
    std::vector<std::string> elegible;
    if (s == "string")
        for (auto const n : names) elegible.push_back(n.first);
    else {
        for (auto const n : names) {
            if (cols[n.second]->type_name() == s) elegible.push_back(n.first);
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

std::vector<std::deque<int>> group_position(const Index& idx) {
    std::vector<std::deque<int>> groupings;
    for (const auto i : idx.get_index_names())
        groupings.push_back(idx.find_index_position(i));
    return groupings;
}

DataFrame::SharedCol col_summary(Statistic* f, const Index& idx,
                          const DataFrame::SharedCol& col) {
    Column new_col = Column(*col, std::deque<int>());
    for (const std::deque<int>& g : group_position(idx))
        new_col.push_back(f->func(g, col));
    return std::make_shared<Column>(new_col);
}

template <class... T>
DataFrame DataFrame::Grouper<T...>::calc(
    Statistic* f, const std::vector<std::string>& names) {
    //DataFrame new_df;
    make_index_unique();
    DataFrame new_df(Index(group_index.index_positions));
    for (const std::string& s : names) {
        SharedCol& col = columns[group_column_names[s]];
        DataFrame::SharedCol res = col_summary(f, group_index, col);
        new_df.append_column(s, res);
    }
    //new_df.index.append_index(group_index.index_positions);
    new_df.assert_same_column_length(__PRETTY_FUNCTION__);
    return new_df;
}

DataFrame DataFrame::summarize(Statistic* f) {
    //DataFrame res;
    std::string name(abi::__cxa_demangle(typeid(*f).name(), 0, 0, 0));
    //std::vector<std::string> new_index;
    //new_index.push_back(name);
    DataFrame res = DataFrame(Index(std::vector<std::string>({name})));
    //res.index = Index(new_index);
    std::vector<std::string> elegible =
        elegible_types(column_names, f->get_name(), columns);
    for (const auto& s : elegible) {
        SharedCol& col = columns[column_names[s]];
        DataFrame::SharedCol summary = col_summary(f, index, col);
        res.append_column(s, summary);
    }
    return res;
}

template <class... T>
DataFrame DataFrame::Grouper<T...>::summarize(Statistic* f) {
    std::vector<std::string> elegible;
    elegible = elegible_types(group_column_names, f->get_name(), columns);
    return calc(f, elegible);
}

template <class... T>
DataFrame DataFrame::Grouper<T...>::summarize(
    Statistic* f, const std::vector<std::string>& names) {
    return calc(f, names);
}
#endif
