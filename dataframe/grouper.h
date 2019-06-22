#ifndef GUARD_Grouper_h
#define GUARD_Grouper_h
#include "dataframe.h"
#include <functional>
#include <numeric>
template <class T>
class DataFrame::Grouper {
   public:
    Grouper(DataFrame&);
    Grouper(DataFrame&, const std::string&);
    static double mean(const std::deque<int>& p, const SharedCol& c) {
        double o = std::accumulate(p.begin(), p.end(), 0.,
            [&](double o, int i) {return o + c->get_value<double>(i);});
        return o / p.size();
    }
    double std(const std::deque<int>&, const DataFrame::SharedCol&);
    DataFrame summarize(double (*f)(const std::deque<int>&,
                                     const DataFrame::SharedCol&));
   private:
    DataFrame& theDataFrame;
    std::unordered_map<std::string, std::deque<int>> old_index_names;
    std::vector<std::string> index_positions;
    std::map<std::string, int> column_names;
};
#endif
