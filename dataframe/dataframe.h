#ifndef GUARD_dataframe_h
#define GUARD_dataframe_h
#include <algorithm>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

class Column;
class DataFrame {
    friend std::ostream& operator<<(std::ostream&, const DataFrame&);

   public:
    DataFrame() = default;
    // I only have column names and double
    // DataFrame(const std::vector<std::string>&);
    DataFrame(const std::vector<std::string>&,
              const std::vector<std::vector<double>>&);
    DataFrame(const std::vector<std::string>&,
              std::vector<std::vector<double>>&&);

    // 5 constructors
    DataFrame(DataFrame&);
    DataFrame& operator=(DataFrame&);
    //~DataFrame(); // how do I deal with this?;
    DataFrame(DataFrame&&);
    DataFrame& operator=(DataFrame&&);

    // operators
    // additional functions
    int size() { return columns.size(); }
    int size() const { return columns.size(); }
    int use_count(std::string);
    void insert(const std::string&, const std::vector<double>&);
    void insert(const std::string&, std::vector<double>&&);

   private:
    std::vector<std::shared_ptr<Column>> columns;  // vector of pointers;
    std::map<std::string, int> column_names;
};

std::ostream& operator<<(std::ostream&, const DataFrame&);
#endif
