#ifndef GUARD_GroupbyFunctions_h
#define GUARD_GroupbyFunctions_h
#include <numeric>
#include "dataframe.h"
#include "dataframeproxy.h"

class Statistic {
   public:
    Statistic() : _return_type("double"), _elegible_type("double") {}
    virtual double func(const std::deque<int>&, const DataFrame::SharedCol&) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    virtual std::string return_type() { return _return_type; }
    virtual std::string elegible_type() { return _elegible_type; }

   protected:
    std::string _return_type;
    std::string _elegible_type;
};

class mean : public Statistic {
   public:
    mean(std::string return_type = "double", 
         std::string elegible_type = "double" ) { 
        _return_type = return_type;
        _elegible_type = elegible_type;}
    double func(const std::deque<int>& pos,
                const DataFrame::SharedCol& col) override {
        double res = std::accumulate(
            pos.begin(), pos.end(), 0.,
            [&](double o, int i) { return o + col->get_value<double>(i); });
        res /= pos.size();
        return res;
    }
    std::string return_type() override { return _return_type; }
    std::string elegible_type() override { return _elegible_type; }
    //std::string get_name() override { return type_name; }
};

class count : public Statistic {
   public:
    count(std::string return_type = "double", 
         std::string elegible_type = "all" ) { 
        _return_type = return_type;
        _elegible_type = elegible_type;}
    double func(const std::deque<int>& pos,
                const DataFrame::SharedCol& col) override {
        (void)col; //avoid GCC unused paramter warining
        return pos.size();
    }
    std::string return_type() override { return _return_type; }
    std::string elegible_type() override { return _elegible_type; }
};

class head : public Statistic {
   public:
    head(std::string return_type = "all", 
         std::string elegible_type = "all" ) { 
        _return_type = return_type;
        _elegible_type = elegible_type;}
    double func(const std::deque<int>& pos,
                const DataFrame::SharedCol& col) override {
        (void)col; //avoid GCC unused paramter warining
        return pos.size();
    }
    std::string return_type() override { return _return_type; }
    std::string elegible_type() override { return _elegible_type; }
};
#endif
