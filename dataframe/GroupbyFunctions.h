#ifndef GUARD_GroupbyFunctions_h
#define GUARD_GroupbyFunctions_h
#include <numeric>
#include "dataframe.h"
#include "dataframeproxy.h"

class Statistic {
   public:
    Statistic() : _return_type("double"), _elegible_type("double") {}
    virtual void func(const std::deque<int>&, const DataFrame::SharedCol&,
                      Column& new_col) {
        new_col.push_back<double>(std::numeric_limits<double>::quiet_NaN());
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
         std::string elegible_type = "double") {
        _return_type = return_type;
        _elegible_type = elegible_type;
    }
    void func(const std::deque<int>& pos, const DataFrame::SharedCol& col,
              Column& new_col) override {
        double res = std::accumulate(
            pos.begin(), pos.end(), 0.,
            [&](double o, int i) { return o + col->get_value<double>(i); });
        res /= pos.size();
        new_col.push_back<double>(res);
    }
    std::string return_type() override { return _return_type; }
    std::string elegible_type() override { return _elegible_type; }
};

class count : public Statistic {
   public:
    count(std::string return_type = "double",
          std::string elegible_type = "all") {
        _return_type = return_type;
        _elegible_type = elegible_type;
    }
    void func(const std::deque<int>& pos, const DataFrame::SharedCol& col,
              Column& new_col) override {
        (void)col;  // avoid GCC unused paramter warining
        new_col.push_back<double>(pos.size());
    }
    std::string return_type() override { return _return_type; }
    std::string elegible_type() override { return _elegible_type; }
};

class head : public Statistic {
   public:
    head(std::string return_type = "all", std::string elegible_type = "all") {
        _return_type = return_type;
        _elegible_type = elegible_type;
    }
    void func(const std::deque<int>& pos, const DataFrame::SharedCol& col,
              Column& new_col) override {
        if (col->type_name() == "string")
            new_col.push_back<std::string>(col->get_value<std::string>(pos[0]));
        else if (col->type_name() == "double")
            new_col.push_back<double>(col->get_value<double>(pos[0]));
        else {
            std::string m("Did not find match the return type, in:\n");
            throw std::invalid_argument(m + __PRETTY_FUNCTION__);
        }
    }
    std::string return_type() override { return _return_type; }
    std::string elegible_type() override { return _elegible_type; }
};
#endif
