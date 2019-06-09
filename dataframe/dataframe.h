#ifndef GUARD_dataframe_h
#define GUARD_dataframe_h
#include <algorithm>
#include <cmath>
#include <map>
#include <memory>
#include <random>
#include <string>
#include <utility>
#include <vector>
#include "column.h"  //need include!

class DataFrame {
   public:
    typedef std::pair<std::string, int> index_pair;
    template <class T>
    class ConstColumnIterator;
    template <class T>
    class ColumnIterator;
    class DataFrameProxy;
    template <class T>
    friend class ConstColumnIterator;
    template <class T>
    friend class ColumnIterator;
    friend DataFrame deep_copy(const DataFrame& lhs);
    template <typename T>
    using const_iter = typename DataFrame::ConstColumnIterator<T>;
    template <typename T>
    using iter = typename DataFrame::ColumnIterator<T>;
    typedef std::shared_ptr<Column> SharedCol;

    DataFrame();
    DataFrame(const DataFrameProxy&);
    template <typename T>
    DataFrame(const std::vector<std::string>&, const std::vector<std::string>&,
              const std::vector<std::vector<T>>&);
    DataFrame& operator=(const DataFrame&);
    friend std::ostream& operator<<(std::ostream&, const DataFrame&);
    friend std::ostream& operator<<(std::ostream&, const DataFrameProxy&);
    friend DataFrame operator+(const DataFrame& lhs, const DataFrame& rhs);
    friend DataFrame operator+(const DataFrameProxy&, const DataFrameProxy&);
    friend DataFrame operator+(const DataFrame&, const DataFrameProxy&);
    template <typename T>
    friend DataFrame operator+(const DataFrame& lhs, const T& t) {
        return deep_copy(lhs) += t;
    }
    friend std::vector<std::pair<int, int>> correspondence_position(
        const DataFrame&, const DataFrame&);
    friend void append_missing_rows(DataFrame&, const DataFrame&);
    friend void append_missing_cols(DataFrame&, const DataFrame&);
    SharedCol get_unique(const std::string&);
    SharedCol get_unique(const std::string&) const;
    SharedCol get_unique(const std::string&, const std::vector<int>&) const;
    SharedCol get_shared_copy(const std::string&);
    SharedCol get_shared_copy(const std::string&) const;
    DataFrame& operator+=(const DataFrame& rhs);
    template <typename T>
    DataFrame& operator+=(const T&);
    // I CANNOT CREATE A NEW COLUMN LIKE THIS!!! MAYE THE CATCH SHOULD CREATE A
    // NEW ONE?? AT LEAST THE NON-CONSTANT VERSION???
    template <class T>
    iter<T> begin(const std::string&);
    template <class T>
    const_iter<T> cbegin(const std::string&);
    template <class T>
    iter<T> end(const std::string&);
    template <class T>
    const_iter<T> cend(const std::string&);
    void reorder_index() {
        auto rng = std::default_random_engine{0};
        std::shuffle(index_names.begin(), index_names.end(), rng);
    }
    void dropna();
    void drop_row(const std::string&);
    void drop_column(const std::string&);
    void sort();
    template <typename T>
    void sort(const std::string&);
    DataFrameProxy operator[](const std::string&);
    DataFrameProxy operator[](const std::vector<std::string>& col_name);
    DataFrameProxy loc(const std::string&);
    const std::pair<int, int> size() const;
    const int use_count(std::string);  // Can i const qualify it?
    std::vector<std::string> get_index_names();
    std::vector<std::string> get_index_names() const;
    std::vector<std::string> get_column_names();
    std::vector<std::string> get_column_names() const;
    template <typename T>
    std::vector<std::string> get_column_names();
    int get_column_position(const std::string&);
    const int find_index_position(const std::string&) const;
    const int find_index_position(const std::string&);
    std::vector<int> get_index_positions() const;
    std::vector<int> get_index_positions(const std::vector<std::string>&) const;

   private:
    std::vector<std::shared_ptr<Column>> columns;
    std::vector<index_pair> index_names;
    std::map<std::string, int> column_names;
    void make_unique_if(const std::string&);
    void make_unique_if(const std::vector<std::string>&);
    void append_nan_rows();
    void append_index(const std::string&);
    std::vector<std::string> frame(Column& c);
    void missing_col_error(const char*, const std::string&);
    bool contains_null(const std::string&);
    void make_unique(const std::string&);
    void make_unique(const std::vector<std::string>&);
    int find_index_pair(const std::pair<std::string, int>&);
    template <class T>
    std::vector<int> permutation_index(const std::string& s);
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
