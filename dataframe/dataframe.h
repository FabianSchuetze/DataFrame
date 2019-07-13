#ifndef GUARD_dataframe_h
#define GUARD_dataframe_h
#include <deque>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "column.h"
#include "index.h"

/**
 *  A test class. A more elaborate class description.
 */
class Index;
class DataFrame {
   public:
    typedef std::pair<std::string, int> index_pair;
    template <class T>
    class const_iterator;
    template <class... T>
    class Grouper;
    template <class T>
    class ColumnIterator;
    class DataFrameProxy;
    template <typename T>
    using iter = typename DataFrame::ColumnIterator<T>;
    typedef std::shared_ptr<Column> SharedCol;

    template <class... T>
    friend class Grouper;
    template <class T>
    friend class const_iterator;
    template <class T>
    friend class ColumnIterator;
    friend DataFrame deep_copy(const DataFrame& lhs);
    friend std::ostream& operator<<(std::ostream&, const DataFrame&);
    friend std::deque<std::pair<int, int>> correspondence_position(
        const DataFrame&, const DataFrame&);
    friend DataFrame operator+(const DataFrame& lhs, const DataFrame& rhs);
    friend DataFrame operator+(const DataFrameProxy&, const DataFrameProxy&);
    friend DataFrame operator+(const DataFrame&, const DataFrameProxy&);
    template <typename T>
    friend DataFrame operator<(const DataFrame& lhs, const T& t) {
        DataFrame copy = deep_copy(lhs);
        for (const auto& x : copy.column_names)
            copy.columns[x.second]->is_smaller_than(t);
        return copy;
    }
    template <typename T>
    friend DataFrame operator>(const DataFrame& lhs, const T& t) {
        DataFrame copy = deep_copy(lhs);
        for (const auto& x : copy.column_names)
            copy.columns[x.second]->is_greater_than(t);
        return copy;
    }
    template <typename T>
    friend DataFrame operator+(const DataFrame& lhs, const T& t) {
        return deep_copy(lhs) += t;
    }

    // Functions
    DataFrame();
    DataFrame(const DataFrameProxy&);  // allow implicit conversion;
    /**
     * @brief Create a new dataframe by reading from an file-stream
     */
    explicit DataFrame(std::ifstream&);
    // I SHOUD REMOVE THE REDUNDANT CONSTRUCTORS!!!
    template <typename T>
    DataFrame(const std::vector<std::string>&, const std::vector<std::string>&,
              const std::vector<std::vector<T>>&);
    template <typename T>
    DataFrame(const Index&, const std::vector<std::string>&,
              const std::vector<std::vector<T>>&);
    template <typename T1, typename... T>
    DataFrame(const std::initializer_list<std::string>&, const std::vector<T1>&,
              const std::vector<T>&...);
    DataFrame& operator=(const DataFrame&);

    /**
     * @brief Returns a shared pointer to a new version of an Column named s
     *
     * The newed unique column has a use_count() of 1. The function is, for
     * example used to deep_copy a dataframe.
     */
    SharedCol get_unique(const std::string&);
    /**
     * @brief for constant members
     */
    SharedCol get_unique(const std::string&) const;
    /**
     * @brief Returns the new column for the subset of indices marked by the
     * vector<int>
     */
    SharedCol get_unique(const std::string&, const std::deque<int>&) const;
    /**
     * @brief The overloaded compound-assignment operator
     *
     * If a colum or row or the rhs is not present in the lhs, a new column or
     * row is created in the lhs dataframe contains nas.
     */
    DataFrame& operator+=(const DataFrame& rhs);
    template <typename T>
    DataFrame& operator+=(const T&);
    template <class T>
    iter<T> begin(const std::string&);
    /**
     * @brief A constant iterator over the columns achored to the beginning of
     * the column
     *
     * THe template arguments needs to equal the datatype of the column
     */
    template <class T>
    const_iterator<T> cbegin(const std::string&);
    template <class T>
    iter<T> end(const std::string&);
    /**
     * @brief A constant iterator over the columns anchored to the end of the
     * column
     *
     * THe template arguments needs to equal the datatype of the column
     */
    template <class T>
    const_iterator<T> cend(const std::string&);
    /**
     * @brief drops rows which contain na from the dataframe
     */
    void dropna();
    // void drop_row(const std::vector<std::deque<Index::ele>>&);
    /**
     * @brief drops a row from the dataframe
     */
    // void drop_row(const std::deque<Index::ele>&);
    /**
     * @brief drops a column from the dataframe
     */
    void drop_column(const std::string&);
    /**
     * @brief Sort the dataframe by its index
     */
    void sort_by_index();
    /**
     * @brief Sorts the dataframe by column named s
     */
    template <typename T1, typename... T2>
    void sort_by_column(const_iterator<T1>, const_iterator<T2>...);
    DataFrameProxy operator[](const std::string&);
    DataFrameProxy operator[](const std::vector<std::string>& col_name);
    // DataFrameProxy loc(const std::string&);
    /**
     * @brief Returns a pair with the row (first) and column (second) numbers
     */
    std::pair<size_t, size_t> size() const;
    int use_count(const std::string&);  // Can i const qualify it?
    // std::vector<std::string> get_index_names();
    // std::vector<std::string> get_index_names() const;
    template <typename T>
    void fill_na(std::string, T);
    /**
     * @brief returns the names of all columns
     */
    std::vector<std::string> get_column_names();
    std::vector<std::string> get_column_names() const;
    /**
     * @brief returns the names of all columns of template type T
     */
    template <typename T>
    std::vector<std::string> get_column_names();
    /**
     * @brief checks if the ordering of underlying data align with the index
     *
     * When the dataframe has been sorted or column have been dropped, the
     * underlying still exist at its old position, it can however not be
     * accessed anymore (through the dataframe) as the index vanished. The
     * function checks whether the data order equals the index ordering. If
     * false, the ordering can be aligned with the function `make_contigious`.
     */
    bool is_contigious();
    /**
     * @brief Rearanges to data in the rows of the columns to be in line with
     * the index again.
     *
     * After the make_contiguous operation, the interface did not change but
     * the position of the data in the columns is again in the same order as
     * the index which might make processing the Dataframe and interating over
     * it faster.
     */
    void make_contigious();
    /**
     * @brief Convert the boolean column name s to a double type
     */
    void convert_bool_to_double(const std::string&);
    /**
     * @brief Groups the dataframe by index and return a grouper object
     */
    // template <class... T>
    // Grouper<T...> groupby();
    /**
     * @brief groups the dataframe by column s and return a grouper object
     */
    template <class... T>
    Grouper<T...> groupby(DataFrame::const_iterator<T>...);

   private:
    std::vector<std::shared_ptr<Column>> columns;
    Index index;
    std::map<std::string, int> column_names;
    void append_duplicate_rows(std::deque<std::pair<int, int>>&);
    void append_duplicate_rows(int);
    void copy_row(int);
    void make_unique_if(const std::string&);
    void make_unique_if(const std::vector<std::string>&);
    void assert_same_column_length(const char*);
    void append_missing_cols(const DataFrame&);
    void append_missing_rows(const DataFrame&);
    void append_nan_rows();
    template <typename T1, typename... T>
    void append_column(const std::vector<std::string>&, int pos,
                       const std::vector<T1>&, const std::vector<T>&...);
    template <typename T1>
    void append_column(const std::vector<std::string>&, int,
                       const std::vector<T1>&);
    /**
     * @brief Appends the string to the end of index_positions and adds it to
     * the hash function index_names
     */
    void append_column(const std::string&, const SharedCol&);
    // void append_index(const std::string&);
    /**
     * @brief Uses the other function as helper function
     */
    // void append_index(const std::vector<std::string>&);
    std::vector<std::string> frame(Column& c);
    std::vector<int> contains_null();
    void make_unique(const std::string&);
    void make_unique(const std::vector<std::string>&);
    /**
     * @brief Returns an index number for the column named s
     * @throws std::out_of_range If the column name does not exist
     */
    int find_column_position(const std::string&);
    int find_column_position(const std::string&) const;
    // std::deque<int> find_index_position() const;
    // std::deque<int> find_index_position();
    /**
     * @brief finds the rows number for the index name given as input
     */
    // std::deque<int> find_index_position(const std::string&) const;
    // std::deque<int> find_index_position(const std::string&);
    /**
     * @Return a shared_ptr to the column named s
     */
    SharedCol get_shared_copy(const std::string&);
    /**
     * @Return a shared_ptr to the column named s for a const-dataframe
     */
    SharedCol get_shared_copy(const std::string&) const;
    /**
     * @brief Initilizes the map `column_names` from the first line of the csv
     * file
     */
    std::vector<std::string> create_column_names(std::ifstream&);
    /**
     * @brief Initilizes the Columns with the datatypes mentiond in the second
     * line of the csv file
     */
    void initialize_column(std::ifstream&, const std::vector<std::string>&);
    /**
     * @fils the previously intilaized columns and creates the index
     */
    template <typename T>
    void initialize_column(const std::string&);
    void insert_data(std::ifstream&, const std::vector<std::string>&);
};

/**
 * @brief adds a value of type T to the dataframe.
 *
 * @throws std::invalid_argument if not all columns of the dataframe are of
 * type T
 */
template <typename T>
DataFrame operator+(const DataFrame&, const T&);
DataFrame operator+(const DataFrame&, const DataFrame&);
DataFrame operator+(const DataFrame&, const DataFrame::DataFrameProxy&);
/**
 * @brief deep copy of a DataFrame
 *
 * Create new instances of all the member columns. The functions is, for
 * example, used in the overloaed + operator.
 */
template <typename T>
DataFrame operator<(const DataFrame&, const T&);
template <typename T>
DataFrame operator>(const DataFrame&, const T&);
DataFrame deep_copy(const DataFrame&);
std::ostream& operator<<(std::ostream&, const DataFrame&);
std::deque<std::pair<int, int>> correspondence_position(const DataFrame&,
                                                        const DataFrame&);
/**
 * @brief creates nan rows in the lhs if rhs cols are not present in the lhs
 *
 * When adding two dataframes rows that are not present in both dataframes
 * generate a new row in the to be created dataframe with missing rows
 */
void append_missing_rows(DataFrame&, const DataFrame&);
/**
 * @brief creates nan cols in the lhs if rhs cols are not present in the lhs
 *
 * When adding two dataframes columns that are not present in both dataframes
 * generate a new column in the to be created dataframe with missing cols
 * Index index;
 */
void append_missing_cols(DataFrame&, const DataFrame&);

// template functions //
template <typename T>
DataFrame::DataFrame(const Index& idx, const std::vector<std::string>& n,
                     const std::vector<std::vector<T>>& cols) {
    if (n.size() != cols.size()) {
        std::string s("#of Input vector differs from # of colum names, in: ");
        throw std::invalid_argument(s + __PRETTY_FUNCTION__);
    }
    for (size_t i = 0; i < n.size(); ++i) {
        if (idx.size() != cols[i].size())
            throw std::invalid_argument("Data and index has different length");
        columns.push_back(std::make_shared<Column>(cols[i]));
        column_names[n[i]] = i;
    }
    index = idx;
}

template <typename T1>
void DataFrame::append_column(const std::vector<std::string>& names, int pos,
                              const std::vector<T1>& v1) {
    append_column(names[pos], std::make_shared<Column>(v1));
}

template <typename T1, typename... T>
void DataFrame::append_column(const std::vector<std::string>& names, int pos,
                              const std::vector<T1>& v1,
                              const std::vector<T>&... v2) {
    append_column(names[pos], std::make_shared<Column>(v1));
    append_column<T...>(names, pos + 1, v2...);
}

template <typename T1, typename... T>
DataFrame::DataFrame(const std::initializer_list<std::string>& names,
                     const std::vector<T1>& v1, const std::vector<T>&... cols)
{
    std::vector<int> idx;
    for (size_t i = 0; i < v1.size(); i++) idx.push_back(i);
    index = Index(idx);
    std::vector<std::string> names_vec(names.begin(), names.end());
    append_column(names_vec[0], std::make_shared<Column>(v1));
    if constexpr (sizeof...(T) > 0) append_column<T...>(names_vec, 1, cols...);
}
#endif
