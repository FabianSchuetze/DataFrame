#ifndef GUARD_Grouper_h
#define GUARD_Grouper_h
#include <functional>
#include <numeric>
#include "dataframe.h"

class Statistic {
   public:
    Statistic() : type_name("double") {}
    virtual double func(const std::deque<int>&, const DataFrame::SharedCol&) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    virtual std::string get_name() { return type_name; }

   protected:
    std::string type_name;
};

class mean : public Statistic {
   public:
    mean(std::string type = "double") { type_name = type; }
    double func(const std::deque<int>& pos,
                const DataFrame::SharedCol& col) override {
        double res = std::accumulate(
            pos.begin(), pos.end(), 0.,
            [&](double o, int i) { return o + col->get_value<double>(i); });
        res /= pos.size();
        return res;
    }
    std::string get_name() override { return type_name; }
};

class count : public Statistic {
   public:
    count(std::string type = "string") { type_name = type; }
    double func(const std::deque<int>& pos,
                const DataFrame::SharedCol& col) override {
        return pos.size();
    }
    std::string get_name() override { return type_name; }
};

template <class T>
class DataFrame::Grouper {
   public:
    Grouper(DataFrame&);
    Grouper(DataFrame&, const std::string&);
    DataFrame summarize(Statistic*);

   private:
    std::vector<std::string> elegible_types(const std::string&);
    DataFrame& theDataFrame;
    std::unordered_map<std::string, std::deque<int>> old_index_names;
    std::vector<std::string> index_positions;
    std::map<std::string, int> column_names;
    void make_index_unique();
};
#endif
