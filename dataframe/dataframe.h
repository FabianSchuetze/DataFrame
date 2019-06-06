#ifndef GUARD_dataframe_h
#define GUARD_dataframe_h
#include <algorithm>
#include <cmath>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "column.h"  //need include!

class DataFrame {
   public:
    typedef std::pair<std::string, int> index_pair;
    class DataFrameProxy;
    friend DataFrame deep_copy(const DataFrame& lhs);
    DataFrame();
    DataFrame(const DataFrameProxy&);
    template <typename T>
    DataFrame(const std::vector<std::string>&, const std::vector<std::string>&,
              const std::vector<std::vector<T>>&);
    DataFrame& operator=(const DataFrame&);
    friend std::ostream& operator<<(std::ostream&, const DataFrame&);
    friend std::ostream& operator<<(std::ostream&, const DataFrameProxy&);
    friend DataFrame operator+(const DataFrame& lhs, const DataFrame& rhs);
    friend DataFrame operator+(const DataFrame::DataFrameProxy&,
                               const DataFrame::DataFrameProxy&);
    friend DataFrame operator+(const DataFrame&,
                               const DataFrame::DataFrameProxy&);
    template <typename T>
    friend DataFrame operator+(const DataFrame& lhs, const T& t) {
        return deep_copy(lhs) += t;
    }
    friend std::vector<std::pair<int, int>> correspondence_position(
        const DataFrame&, const DataFrame&);
    friend void append_missing_rows(DataFrame&, const DataFrame&);
    friend void append_missing_cols(DataFrame&, const DataFrame&);
    std::shared_ptr<Column> get_unique(const std::string&);
    std::shared_ptr<Column> get_unique(const std::string&) const;
    std::shared_ptr<Column> get_unique(const std::string&,
                                       const std::vector<int>&) const;
    std::shared_ptr<Column> get_shared_copy(const std::string&);
    std::shared_ptr<Column> get_shared_copy(const std::string&) const;
    DataFrame& operator+=(const DataFrame& rhs);
    template <typename T>
    DataFrame& operator+=(const T&);
    template <class T>
    typename std::vector<T>::iterator begin(std::string);
    template <class T>
    typename std::vector<T>::iterator end(std::string);
    // template <class T> typename std::vector<T>::const_iterator
    // begin(std::string);
    template <class T>
    typename std::vector<T>::const_iterator end(std::string);
    // what shall I do with the constant operator?
    DataFrameProxy operator[](const std::string&);
    DataFrameProxy operator[](const std::vector<std::string>& col_name);
    DataFrameProxy loc(const std::string&);
    const std::pair<int, int> size() const;
    const int use_count(std::string);  // Can i const qualify it?
    std::vector<std::string> get_index_names();
    std::vector<std::string> get_column_names();
    const int find_index_position(const std::string&) const;
    const int find_index_position(const std::string&);

   private:
    std::vector<std::shared_ptr<Column>> columns;
    std::vector<index_pair> index_names;
    std::map<std::string, int> column_names;
    void make_unique_if(const std::string&);
    void append_nan_rows();
    void append_index(const std::string&);
};

template <typename T>
DataFrame operator+(const DataFrame&, const T&);
DataFrame operator+(const DataFrame&, const DataFrame&);
DataFrame operator+(const DataFrame&, const DataFrame::DataFrameProxy&);
DataFrame deep_copy(const DataFrame&);
std::ostream& operator<<(std::ostream&, const DataFrame&);
std::vector<std::pair<int, int>> correspondence_position(const DataFrame&,
                                                         const DataFrame&);
void append_missing_rows(DataFrame&, const DataFrame&);
void append_missing_cols(DataFrame&, const DataFrame&);
#endif
