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
    /**
     * @brief Constructor for the grouper
     * @param df The dataframe which is grouped
     * @param its The iteratos indication according to which columns the
     * dataframe shall be grouped.
     */
    // WHAT DO I DO IF THE ITERATOR HAS A DIFFERENT PARENT??//
    Grouper(DataFrame&, DataFrame::const_iterator<T>&...);
    /**
     * @brief returns a dataframe with the calculated summary statistics.
     * @param fun A derived class of Statistic; the function used to cuculate
     * the grouped-by values.
     *
     * The new index reflects the underlying grouping. Which derived class of
     * Statistic is used determines which columns are used.
     */
    DataFrame summarize(Statistic* fun);
    /**
     * @brief returns a dataframe with the calculated summary statistics.
     * @param fun A derived class of Statistic; the function used to cuculate
     * the grouped-by values.
     * @param vec A vector with the columns over which the statistic shall be
     * calculated.
     *
     */
    DataFrame summarize(Statistic*, const std::vector<std::string>&);

   private:
    std::vector<SharedCol> columns;
    Index group_index;
    std::map<std::string, int> group_column_names;

    // Functions
    DataFrame calc(Statistic*, const std::vector<std::string>&);
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
    std::vector<T> res(df.cbegin<T>(n), df.cend<T>(n));
    return res;
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
    test_equality<T...>(it...);
    DataFrame df_copy = *this;
    df_copy.sort_by_column_no_check<T...>(it...);
    Grouper<T...> grouper(df_copy, it...);
    return grouper;
}

std::vector<std::string> elegible_types(
    const std::map<std::string, int>& names, const std::string& s,
    const std::vector<DataFrame::SharedCol>& cols) {
    std::vector<std::string> elegible;
    if ((s == "string") | (s == "all"))
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

Column provide_empty_column(Statistic* f, const DataFrame::SharedCol& col) {
    Column new_col;
    if (f->return_type() == "double")
        new_col = Column(std::vector<double>());
    else if (f->return_type() == "all")
        new_col = Column(*col, std::deque<int>());
    else if (f->return_type() == "string")
        new_col = Column(std::vector<std::string>());
    else {
        std::string m("Cannot deal with argument, in:\n");
        throw std::invalid_argument(m + __PRETTY_FUNCTION__);
    }
    return new_col;
}

DataFrame::SharedCol col_summary(Statistic* f,
                                 const std::vector<std::deque<int>>& groupings,
                                 const DataFrame::SharedCol& col) {
    Column new_col = provide_empty_column(f, col);
    for (const std::deque<int>& g : groupings) f->func(g, col, new_col);
    return std::make_shared<Column>(new_col);
}

template <class... T>
DataFrame DataFrame::Grouper<T...>::calc(
    Statistic* f, const std::vector<std::string>& names) {
    make_index_unique();
    DataFrame new_df(Index(group_index.index_positions));
    std::vector<std::deque<int>> groupings = group_position(group_index);
    for (const std::string& s : names) {
        SharedCol& col = columns[group_column_names[s]];
        DataFrame::SharedCol res = col_summary(f, groupings, col);
        new_df.append_column(s, res);
    }
    new_df.assert_same_column_length(__PRETTY_FUNCTION__);
    return new_df;
}

DataFrame DataFrame::summarize(Statistic* f) {
    std::string name(abi::__cxa_demangle(typeid(*f).name(), 0, 0, 0));
    DataFrame res = DataFrame(Index(std::vector<std::string>({name})));
    std::vector<std::deque<int>> groupings({index.find_index_position()});
    for (auto& s : elegible_types(column_names, f->elegible_type(), columns)) {
        SharedCol& col = columns[column_names[s]];
        DataFrame::SharedCol summary = col_summary(f, groupings, col);
        res.append_column(s, summary);
    }
    return res;
}

template <class... T>
DataFrame DataFrame::Grouper<T...>::summarize(Statistic* f) {
    std::vector<std::string> elegible;
    elegible = elegible_types(group_column_names, f->elegible_type(), columns);
    return calc(f, elegible);
}

template <class... T>
DataFrame DataFrame::Grouper<T...>::summarize(
    Statistic* f, const std::vector<std::string>& names) {
    std::string valid(f->elegible_type());
    for (const std::string& name : names) {
        std::string col_type(columns[group_column_names[name]]->type_name());
        if ((valid != "all") & (valid != col_type)) {
            std::string m("The column " + name + " has invalid types, in:\n");
            throw std::invalid_argument(m + __PRETTY_FUNCTION__);
        }
    }
    return calc(f, names);
}
#endif
