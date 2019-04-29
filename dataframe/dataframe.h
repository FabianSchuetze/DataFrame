#ifndef GUARD_dataframe_h
#define GUARD_dataframe_h
#include <string>
#include <vector>
#include <map>

class DataFrame {
   public:
    typedef std::vector<double> data_col;
    DataFrame(){};
    DataFrame(const std::vector<std::string>& index);
    DataFrame(const std::vector<std::string>& index,
              const std::vector<std::string>& columns,
              const std::vector<data_col>& data);
    // Do I need copy constcuttore, desctrctor etc?

    // should all return a DataFrame!!! How can I do this?!
    const std::vector<double> operator()(const std::string&);
    double operator()(const std::string&, const std::string&);
    // not correct, you would want a df back;

   private:
    std::map<std::string, int> index;
    std::map<std::string, int> columns;
    std::vector<data_col*> data; //vector of pointers;

    typedef std::vector<std::string>::const_iterator string_iter;

    string_iter find_index(const string_iter, const string_iter,
                           const std::string&);
    void init_map(const std::vector<std::string>, std::map<std::string, int>&);
};
// I want this to return a dataframe as well!!!
double operator+(const DataFrame&, const DataFrame&);

#endif
