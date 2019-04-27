#ifndef GUARD_dataframe_h
#define GUARD_dataframe_h
#include <string>
#include <vector>

class DataFrame {
   public:
    DataFrame(){};
    DataFrame(const std::vector<std::string>&);
    DataFrame(const std::vector<std::string>&, const std::vector<std::string>&,
              const std::vector<std::vector<double>>&);
    // Do I need copy constcuttore, desctrctor etc?

    // should all return a DataFrame!!! How can I do this?!
    const std::vector<double> operator()(const std::string&);
    double operator()(const std::string&, const std::string&);
    // not correct, you would want a df back;

   private:
    std::vector<std::string> index;
    std::vector<std::string> names;
    std::vector<std::vector<double>> columns;

    typedef std::vector<std::string>::const_iterator string_iter;

    string_iter find_index(const string_iter, const string_iter,
                           const std::string&);
};
// I want this to return a dataframe as well!!!
double operator+(const DataFrame&, const DataFrame&);

#endif
