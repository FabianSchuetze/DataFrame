#ifndef GUARD_dataframe_h
#define GUARD_dataframe_h
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>

class Column;
class DataFrame {

   public:
    DataFrame();
    // I only have column names and double
    DataFrame(const std::vector<std::string>&);
    DataFrame(const std::vector<std::string>&,
              std::vector<std::vector<double> >&);
    DataFrame(const std::vector<std::string>&,
              std::vector<std::vector<double> >&&);

    //5 constructors
    DataFrame(const DataFrame&);
    DataFrame& operator=(const DataFrame&);
    ~DataFrame(); // how do I deal with this?;
    DataFrame(DataFrame&&);
    DataFrame& operator=(DataFrame&&);

    int count();
    // insert a new column
    void insert(const std::vector<std::string>&, const std::vector<double>&);
    void insert(std::vector<std::string>&&, std::vector<double>&&);

   private:
    std::vector<std::shared_ptr<Column>> columns;  // vector of pointers;
    std::map<std::string, int> column_names;
};
#endif
