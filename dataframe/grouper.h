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
    Grouper(DataFrame&, const std::string&);
    DataFrame summarize(Statistic*);

   private:
    std::vector<std::string> elegible_types(const std::string&);
    std::vector<SharedCol> columns;
    std::map<std::deque<Index::ele>, std::deque<int>> old_index_names;
    std::vector<std::deque<Index::ele>> index_positions;
    std::map<std::string, int> column_names;
    void make_index_unique();
};
#endif
