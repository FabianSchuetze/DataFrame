#ifndef GUARD_Grouper_h
#define GUARD_Grouper_h
#include <functional>
#include <numeric>
#include "dataframe.h"
#include "dataframeproxy.h"
#include "GroupbyFunctions.h"

template <class... T>
class DataFrame::Grouper {
   public:
    Grouper(DataFrame&);
    Grouper(DataFrame&, DataFrame::const_iter<T>&...);
    DataFrame summarize(Statistic*);

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

template <typename T1,  typename... T>
void reset_iterator(DataFrame::const_iter<T1> it1,
                    DataFrame::const_iter<T>... it) {
    it1.reset();
    if constexpr (sizeof...(T) > 0) reset_iterator<T...>(it...);
}

//// NOT A GOOD STYLE!
template <typename T>
std::vector<T> return_vector(std::string n, DataFrame& df) {
    std::vector<T>res;
    DataFrame::const_iter<T> it = df.cbegin<T>(n);
    while (it != df.cend<T>(n)) res.push_back(*it++);
    return res;
}

template <typename... T>
DataFrame::Grouper<T...>::Grouper(DataFrame& a, DataFrame::const_iter<T>&... b)
    : columns(a.columns) {
    std::deque<int> old_index_positions = a.index.find_index_position();
    group_index = Index(old_index_positions,
                        return_vector<T>(b.return_name(), a)...);
    group_column_names = a.column_names;
    //group_column_names.erase(b.return_name())...;
}

template <typename... T>
DataFrame::Grouper<T...> DataFrame::groupby(DataFrame::const_iter<T>... it) {
    DataFrame df_copy = *this;
    df_copy.sort_by_column<T...>(it...);
    Grouper<T...> grouper(df_copy, it...);
    return grouper;
}
template <class... T>
std::vector<std::string>
DataFrame::Grouper<T...>::elegible_types(const std::string& s) {
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
    auto it =
        std::unique(group_index.index_positions.begin(),
                     group_index.index_positions.end());
    group_index.index_positions.erase(it, group_index.index_positions.end());
}

template <class... T>
DataFrame DataFrame::Grouper<T...>::summarize(Statistic* f) {
    DataFrame res;
    make_index_unique();
    for (const std::string& s : elegible_types(f->get_name())) {
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
#endif
