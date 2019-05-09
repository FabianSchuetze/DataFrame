#ifndef GUARD_dataframe_h
#define GUARD_dataframe_h
#include <map>
#include <string>
#include <vector>
#include <algorithm>

class DataFrame {
    friend DataFrame operator+(const DataFrame&, const DataFrame&);

   public:
    typedef double* data_col;
    DataFrame() = default;
    DataFrame(std::vector<std::string>&);
    DataFrame(std::vector<std::string>&, std::vector<std::string>&,
              std::vector<std::vector<double> >&);
    DataFrame(std::map<std::string, int>&, 
              std::vector<std::string>&,
              std::map<std::string, int>&,
              std::vector<std::string>&);

    std::map<std::string, int> get_columns() { return columns; }
    std::map<std::string, int> get_index() { return index; }
    std::vector<std::string> get_rev_columns() { return rev_columns; }
    std::vector<std::string> get_rev_index() { return rev_index; }
    // std::vector<data_col> get_data() const { return data; }

    // Do I need copy constcuttore, desctrctor etc?
    DataFrame& operator() (const std::vector<std::string>&) const;
    DataFrame& operator() (const std::vector<std::string>&);
    DataFrame& operator=(const DataFrame&);

    void print(int);
    void print() const;

    // insert a new column
    void insert(const std::vector<std::string>&, 
                const std::vector<double>&);

    void sort_index() {std::sort(rev_index.begin(), rev_index.end());}

   private:
    // double* data;
    std::vector<double*> data;  // vector of pointers;
    std::map<std::string, int> index;
    std::vector<std::string> rev_index;
    std::map<std::string, int> columns;
    std::vector<std::string> rev_columns;

    std::vector<int> find_subset(std::map<std::string, int>&,
                                 std::vector<std::string>&,
                                 const std::vector<std::string>&,
                                 const std::map<std::string, int>&) const;
    int find_index(const std::map<std::string, int>&, const std::string&) const;
    void init_map(const std::vector<std::string>&, std::map<std::string, int>&,
                  std::vector<std::string>&);
};
// non-member functions
// DataFrame operator+(const DataFrame&, const DataFrame&);

#endif
