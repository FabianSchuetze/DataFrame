#ifndef GUARD_dataframe_h
#define GUARD_dataframe_h
#include <algorithm>
#include <cmath>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "column.h"

class DataFrame {
   public:
    typedef std::pair<std::string, int> index_pair;
    class DataFrameProxy;
    DataFrame();
    DataFrame(const DataFrameProxy&);
    template <typename T>
    DataFrame(const std::vector<std::string>&, const std::vector<std::string>&,
              const std::vector<std::vector<T>>&);
    DataFrame(const std::vector<index_pair>&,const std::map<std::string, int>&,
              const std::vector<std::shared_ptr<Column>>&);
    DataFrame& operator=(const DataFrame&);
    //friend class DataFrameProxy; // DO I need this?
    friend std::ostream& operator<<(std::ostream&, const DataFrame&);
    friend std::ostream& operator<<(std::ostream&, const DataFrameProxy&);
    friend DataFrame operator+(const DataFrame& lhs, const DataFrame& rhs);
    friend DataFrame operator+(const DataFrame::DataFrameProxy&,
                               const DataFrame::DataFrameProxy&);
    friend DataFrame operator+(const DataFrame&,
                               const DataFrame::DataFrameProxy&);
    template <typename T>
    friend DataFrame operator+(const DataFrame& lhs, const T& t) {
        return DataFrame(lhs.index_names, lhs.column_names, lhs.columns) += t;
    }
    friend std::vector<std::pair<int, int>> correspondence_position(
        const DataFrame&, const DataFrame&);
    friend void append_missing_rows(DataFrame&, const DataFrame&);
    friend void append_missing_cols(DataFrame&, const DataFrame&);

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
    const int use_count(std::string); //Can i const qualify it?

   private:
    std::vector<std::shared_ptr<Column>> columns;
    std::vector<index_pair> index_names;
    std::map<std::string, int> column_names;
    void make_unique_if(const std::string&);
    void get_index_names(std::vector<std::string>&);
};

template <typename T>
DataFrame operator+(const DataFrame&, const T&);
DataFrame operator+(const DataFrame&, const DataFrame&);
DataFrame operator+(const DataFrame&, const DataFrame::DataFrameProxy&);
std::ostream& operator<<(std::ostream&, const DataFrame&);
std::vector<std::pair<int, int>> correspondence_position(const DataFrame&,
                                                         const DataFrame&);
// const qualify?
void append_missing_rows(DataFrame&, const DataFrame&);
void append_missing_cols(DataFrame&, const DataFrame&);
const int find_position(const std::string&,
                        const std::vector<DataFrame::index_pair>& vec);
#endif
