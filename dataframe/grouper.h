#ifndef GUARD_Grouper_h
#define GUARD_Grouper_h
#include <functional>
#include <numeric>
#include "dataframe.h"
#include "dataframeproxy.h"
#include "GroupbyFunctions.h"

// I NEED MULTIPLE TYPES!!!
template <class T>
class DataFrame::Grouper {
   public:
    Grouper(DataFrame&);
    Grouper(DataFrame&, DataFrame::const_iter<T>&);
    DataFrame summarize(Statistic*);

   private:
    std::vector<std::string> elegible_types(const std::string&);
    std::vector<SharedCol> columns;
    std::map<std::deque<Index::ele>, std::deque<int>> old_index_names;
    std::vector<std::deque<Index::ele>> index_positions;
    std::map<std::string, int> column_names;
    void make_index_unique();
};
template <class T>
DataFrame::Grouper<T>::Grouper(DataFrame& a)
    : columns(a.columns),
      old_index_names(a.index.index_names),
      index_positions(a.index.index_positions),
      column_names(a.column_names) {}

template <typename T>
DataFrame::Grouper<T>::Grouper(DataFrame& a, DataFrame::const_iter<T>& b)
    : columns(a.columns) {
    std::deque<int> old_index_positions = a.index.find_index_position();
    for (size_t i = 0; i < old_index_positions.size(); i++) {
    //int i = 0;
    //for (const_iter<T> b = a.cbegin<T>(s); b != a.cend<T>(s); ++b) {
        std::deque<Index::ele> tmp{*b++};
        index_positions.push_back(tmp);
        old_index_names[tmp].push_back(old_index_positions[i++]);
    }
    column_names = a.column_names;
    // NEED TO DO THAT SOON
    //column_names.erase(s);
}

template <class T>
DataFrame::Grouper<T> DataFrame::groupby(DataFrame::const_iter<T> s) {
    sort_by_column<T>(s);
    Grouper<T> grouper(*this, s);
    return grouper;
}
#endif
