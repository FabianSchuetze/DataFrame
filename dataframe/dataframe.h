#ifndef GUARD_dataframe_h
#define GUARD_dataframe_h
#include <map>
#include <string>
#include <vector>

class DataFrame {
    friend DataFrame operator+(const DataFrame&, const DataFrame&);

   public:
    typedef double* data_col;
    DataFrame(){};
    DataFrame(const std::vector<std::string>&);
    DataFrame(const std::vector<std::string>&, const std::vector<std::string>&,
              std::vector<std::vector<double> >&);
    DataFrame(const std::map<std::string, int>&, 
              const std::map<int, std::string>&,
              const std::map<std::string, int>&,
              const std::map<int, std::string>&);

    std::map<std::string, int> get_columns() const { return columns; }
    std::map<std::string, int> get_index() const { return index; }
    std::map<int, std::string> get_rev_columns() const { return rev_columns; }
    std::map<int, std::string> get_rev_index() const { return rev_index; }
    // std::vector<data_col> get_data() const { return data; }

    // Do I need copy constcuttore, desctrctor etc?

    DataFrame operator()(const std::vector<std::string>&);
    // DataFrame operator()(const std::string&, const std::string&);
    void print();

   private:
    // double* data;
    std::vector<double*> data;  // vector of pointers;
    std::map<std::string, int> index;
    std::map<int, std::string> rev_index;
    std::map<std::string, int> columns;
    std::map<int, std::string> rev_columns;

    std::vector<int> find_subset(std::map<std::string, int>&,
                                 std::map<int, std::string>&,
                                 const std::vector<std::string>&,
                                 const std::map<std::string, int>&) const;
    int find_index(const std::map<std::string, int>&, const std::string&) const;
    void init_map(const std::vector<std::string>&, std::map<std::string, int>&,
                  std::map<int, std::string>&);
};
// non-member functions
// DataFrame operator+(const DataFrame&, const DataFrame&);

#endif
