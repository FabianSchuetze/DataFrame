#ifndef GUARD_Grouper_h
#define GUARD_Grouper_h
#include "dataframe.h"
template <class T>
class DataFrame::Grouper {
   public:
    Grouper(DataFrame&);
    Grouper(DataFrame&, const std::string&);
    DataFrame mean();
   private:
    DataFrame& theDataFrame;
    std::unordered_map<std::string, std::deque<int>> old_index_names;
    std::vector<std::string> index_positions;
    std::map<std::string, int> column_names;
};
#endif
