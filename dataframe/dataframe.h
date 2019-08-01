#ifndef GUARD_dataframe_h
#define GUARD_dataframe_h
#include <deque>
#include <fstream>
#include <map>
#include <memory>
#include <numeric>
#include <string>
#include <utility>
#include <vector>
#include "column.h"
#include "index.h"
//#include "GroupbyFunctions.h"

/**
 *  A test class. A more elaborate class description.
 */
class Index;
class Statistic;
class DataFrame {
   public:
    typedef std::pair<std::string, int> index_pair;
    template <class T>
    class const_iterator;
    template <class... T>
    class Grouper;
    template <class T>
    class iterator;
    class DataFrameProxy;
    typedef std::shared_ptr<Column> SharedCol;
friend class Column;

friend DataFrame deep_copy(const DataFrame& lhs);
friend std::ostream& operator<<(std::ostream&, const DataFrame&);
friend DataFrame operator+(const DataFrame& lhs, const DataFrame& rhs);
friend DataFrame operator+(const DataFrameProxy&, const DataFrameProxy&);
friend DataFrame operator+(const DataFrame&, const DataFrameProxy&);
template <typename T>
friend DataFrame operator<(const DataFrame& lhs, T t);
template <typename T>
friend DataFrame operator>(const DataFrame& lhs, T t);
template <typename T>
friend DataFrame operator+(const DataFrame& lhs, const T& t) {
    return deep_copy(lhs) += t;
}

// Functions
/**
 * @brief Default Constructor
 */
DataFrame();
/**
 * @brief converts a ProxyClass into the parent DataFrame
 *
 * When selecting a subset of columns of a DataFrame based on the subscript
 * operator, a ProxyClass is created. This ProxyClass faciliates either
 * adding new columns to the dataframe or reading columns from the subset.
 */
DataFrame(const DataFrameProxy&);  // allow implicit conversion;
/**
 * @brief Create a new dataframe by reading from an file-stream from
 * harddisk.
 *
 * At the moment, the datatypes of the column cannot be inferred. Instead
 * the datatypes are given as the second line of the csv file. An
 * additional restriction is that the first column is assumed to be the
 * index column.
 */
explicit DataFrame(std::ifstream&);
/**
 * @brief constructor of the dataframe with only an index
 */
DataFrame(const Index&);
/**
 * @brief Create dataframe with an index, the column names and vector of
 * different datatype
 */
template <typename T1, typename... T>
DataFrame(const Index&, const std::vector<std::string>&,
          const std::vector<T1>&, const std::vector<T>&...);
/**
 * @brief Create dataframe without index, the column names and vector of
 * different datatype.
 *
 * As no index is passed, a default index will be created. The default
 * index are integers from 0 to the number of lines passed.
 */
template <typename T1, typename... T>
DataFrame(const std::vector<std::string>&, const std::vector<T1>&,
          const std::vector<T>&...);
/**
 * @brief Copy assignment operator
 *
 * The copy assignment operator makes a copy of the dataframe. The index
 * and column names are a deep-copy. The data is shared between the two
 * dataframes. When attempting to modify a column in a dataframe,
 * copy-on-write is used. This technique avoids having to rely on defensive
 * copying.
 */
DataFrame& operator=(const DataFrame&) = default;

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
 * deque<int>.
 */
SharedCol get_unique(const std::string&, const std::deque<int>&) const;
/**
 * @brief The overloaded compound-assignment operator
 *
 * If a colum or row or the rhs is not present in the lhs, a new column or
 * row is created in the lhs dataframe containing nas.
 */
/**
 *
 */
DataFrame summarize(Statistic* f);
DataFrame& operator+=(const DataFrame& rhs);
/**
 * @brief Componound assignment operator assing object of type double, int
 * or string to the dataframe
 */
// WHAT HAPPENS WITH COLUMNS THAT ARE NOT OF THIS TYPE?
template <typename T>
DataFrame& operator+=(const T&);
/**
 * @brief A bidirectional-iterator over the columns
 *
 * Asking for the iterator (instead of the constant iterator) can generate
 * a new vector with the original column elements when more than one
 * dataframe reference the column (A copy-on-write is assumed.)
 */
template <class T>
iterator<T> begin(const std::string&);
/**
 * @brief A constant bidirectional-iterator over the columns
 * achored to the beginning of the column
 *
 * THe template arguments needs to equal the datatype of the column
 */
template <class T>
const_iterator<T> cbegin(const std::string&);
/**
 * @brief Reference the last element in the column
 */
template <class T>
iterator<T> end(const std::string&);
/**
 * @brief A constant iterator over the columns anchored to the end of the
 * column
 *
 * THe template arguments needs to equal the datatype of the column
 */
template <class T>
const_iterator<T> cend(const std::string&);
/**
 * @brief Returns a copy of the particular element requested
 */
/**
 * @brief drops rows which contain na from the dataframe
 *
 * In case the column is of type double or int, nan are unambigious. For
 * columns of string type, na are pressumed to be 'NA'.
 */
/**
 * @brief Checks if the const_iterator was create from the instantiation of
 * the dataframe
 *
 * The sorting and grouping function acccess a ConstColumIterator as an
 * argument. In theory, this iterator could be originated form a different
 * dataframe. To prevent passing such arguments, the function checks
 * whether the iterator is reference to the same argument on which the sort
 * and grouping function is called
 */
template <typename T1, typename... T2>
void test_belonging(const_iterator<T1>&, const_iterator<T2>&...);
/**
 * @brief Checks if the iterator was create from the instantiation of
 * the dataframe
 */
template <typename T1, typename... T2>
    void test_belonging(iterator<T1>&, iterator<T2>&...);
    /**
     * @brief Drops rows which contains na values
     *
     * Strings are assumed to be of NA type if their values if `NA`.
     */
    void dropna();
    /**
     * @brief Drops rows based on the index of the dataframe
     *
     * Given the vector of indices of the dataframe, the rows of the dataframe
     * are dropped. If the arguments are not in the index, the dataframe is the
     * same as before the function is called.
     */
    void drop_row(std::vector<std::deque<Index::ele>>);
    /**
     * @brief drops a row from the dataframe
     */
    void drop_row(std::deque<Index::ele>);
    /**
     * @brief drops a column from the dataframe
     */
    void drop_column(const std::string&);
    /**
     * @brief drops a column from the dataframe
     */
    void drop_column(const std::vector<std::string>&);
    /**
     * @brief Sort the dataframe by its index
     */
    void sort_by_index();
    /**
     * @brief Sorts the dataframe by column named s
     */
    template <typename T1, typename... T2>
    void sort_by_column(const_iterator<T1>, const_iterator<T2>...);
    /**
     * @brief Returns a ProxyClass of the DataFrame which can then be used to
     * overload functions upon the return value (see Scott Mayers, More
     * Effective C++
     * @param s A column name
     */
    DataFrameProxy operator[](const std::string& s);
    /**
     * @brief Returns a ProxyClass of the DataFrame which can then be used to
     * overload functions upon the return value (see Scott Mayers, More
     * Effective C++
     * @param vec A vector of column names
     */
    DataFrameProxy operator[](const std::vector<std::string>& vec);
    /**
     * @brief Returns a ProxyClass of the DataFrame which can then be used to
     * overload functions upon the return value (see Scott Mayers, More
     * Effective C++
     * @param idx One index (one row of the indices)
     * @param s One column name
     */
    DataFrameProxy loc(const std::deque<Index::ele>& idx, const std::string& s);
    /**
     * @brief Returns a ProxyClass of the DataFrame which can then be used to
     * overload functions upon the return value (see Scott Mayers, More
     * Effective C++
     * @param idx Several index elements given in a vector
     * @param vec Several column names
     */
    DataFrameProxy loc(const std::vector<std::deque<Index::ele>>& idx,
                       const std::vector<std::string>& vec);
    /**
     * @brief Returns a pair with the row (first) and column (second) numbers
     */
    std::pair<size_t, size_t> size() const;
    /**
     * @bried Returns the number of dataframes referencing to the column with
     * name s.
     *
     * The use_count is employed for the copy-on-write functionality of teh
     * dataframe. If more than one dataframe references the column and the
     * column is about to be altered (or a iterator (not const_iterator) is
     * requested) the column is copied.
     */
    int use_count(const std::string& s);  // Can i const qualify it?
    /**
     * @brief Fills the na values of column s with values of type t
     */
    template <typename T>
    void fill_na(std::string s, T t);
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
    // Data Memebers
    std::vector<std::shared_ptr<Column>> columns;
    Index index;
    std::map<std::string, int> column_names;

    // Functions
    template <typename T1, typename... T2>
    void sort_by_column_no_check(const_iterator<T1>, const_iterator<T2>...);
    /**
     * @brief If some indices occur more than once in correspondence, new rows
     * are append to the dataset
     *
     * The function is used in the compound assignment operator. When the rhs
     * has a index which appears multiple times but it appears only once at the
     * left hand side, the a new row in the lhs is created so that the addition
     * can proceed.
     */
    void duplicate_rows(std::deque<std::pair<int, int>>& correspondence);
    /**
     * @brief If some indices occur more than once in correspondence, new rows
     * are append to the dataset
     *
     * The function is used in the compound assignment operator. When the rhs
     * has a index which appears multiple times but it appears only once at the
     * left hand side, the a new row in the lhs is created so that the addition
     * can proceed.
     */
    void duplicate_rows(int);
    void copy_row(int);
    void make_unique_if(const std::string&);
    void make_unique_if(const std::vector<std::string>&);
    void assert_same_column_length(const char*);
    /**
     * @brief Expands the dataframe by the columns that are in other but not in
     * self
     * @param other The other dataframe whose columns used as comparison
     */
    void append_missing_cols(const DataFrame& other);
    /**
     * @brief Expends the rows of the dataframe by the rows that ar in other
     * but not in self
     * @param other The othe dataframge whose rows are used as comparison
     */
    void append_missing_rows(const DataFrame&);
    /**
     * @brief Appends an nan value to the all column in the dataframe.
     *
     * The dataframe must have at least one element, otherwise it doesn't
     * work?!?
     */
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
     * @brief Helper function to create the column names
     */
    void create_column_names(const std::string&, const char*);
    /**
     * @brief Initilizes the Columns with the datatypes mentiond in the second
     * line of the csv file
     */
    std::string initialize_column(std::ifstream&,
                                  const std::vector<std::string>&);
    /**
     * @bried fills the previously intilaized columns and creates the index
     */
    template <typename T>
    void initialize_column(const std::string&);
    /**
     * @brief helper function.
     */
    void initialize_column(const std::string&, const std::string&);
    void insert_data(std::ifstream&, const std::vector<std::string>&,
                     const std::string&);
};

/**
 * @brief adds a value of type T to the dataframe.
 *
 * @throws std::invalid_argument if not all columns of the dataframe are of
 * type T
 */
//template <typename T>
//DataFrame operator+(const DataFrame&, const T&);
//DataFrame operator+(const DataFrame&, const DataFrame&);
//DataFrame operator+(const DataFrame&, const DataFrame::DataFrameProxy&);
/**
 * @brief deep copy of a DataFrame
 *
 * Create new instances of all the member columns. The functions is, for
 * example, used in the overloaed + operator.
 */
// template <typename T>
// DataFrame operator<(const DataFrame&, const T&);
// template <typename T>
// DataFrame operator>(const DataFrame&, const T&);
DataFrame deep_copy(const DataFrame&);
std::ostream& operator<<(std::ostream&, const DataFrame&);
// std::deque<std::pair<int, int>> correspondence_position(const DataFrame&,
// const DataFrame&);
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

void equal_width(size_t num_names, size_t, const char*);

template <typename T1, typename... T>
void equal_length(size_t len_idx, const char* fun, const std::vector<T1>& v1,
                  const std::vector<T>&... v) {
    if (!(len_idx == v1.size())) {
        std::string s("The index lenght is: " + std::to_string(len_idx) +
                      " but the input vector has length " +
                      std::to_string(v1.size()) + "in:\n");
        throw std::invalid_argument(s + fun);
    }
    if constexpr (sizeof...(T) > 0) equal_length<T...>(len_idx, fun, v...);
}

template <typename T1, typename... T>
DataFrame::DataFrame(const Index& idx, const std::vector<std::string>& names,
                     const std::vector<T1>& v1, const std::vector<T>&... v) {
    equal_width(names.size(), sizeof...(T) + 1, __PRETTY_FUNCTION__);
    index = idx;
    equal_length(index.size().first, __PRETTY_FUNCTION__, v1, v...);
    append_column(names[0], std::make_shared<Column>(v1));
    if constexpr (sizeof...(T) > 0) append_column<T...>(names, 1, v...);
}

template <typename T1, typename... T>
DataFrame::DataFrame(const std::vector<std::string>& names,
                     const std::vector<T1>& v1, const std::vector<T>&... cols) {
    equal_width(names.size(), sizeof...(T) + 1, __PRETTY_FUNCTION__);
    std::vector<int> idx(v1.size());
    std::iota(idx.begin(), idx.end(), 0);
    index = Index(idx);
    append_column(names[0], std::make_shared<Column>(v1));
    if constexpr (sizeof...(T) > 0) append_column<T...>(names, 1, cols...);
}

/**
 * @brief Compares a dataframe with a elemary values emelemt-wise and returns a
 * new dataframe with boolean values
 */
template <typename T>
DataFrame operator<(const DataFrame& lhs, T t) {
    if constexpr ((std::is_same<T, double>::value |
                   std::is_same<T, std::string>::value)) {
        DataFrame copy = deep_copy(lhs);
        for (const auto& x : copy.column_names)
            copy.columns[x.second]->is_smaller_than(t, x.first);
        return copy;
    }
    std::string m("The function accepts only double and strings, in:\n");
    throw std::invalid_argument(m + __PRETTY_FUNCTION__);
}

/**
 * @brief Compares a dataframe with a elemary values emelemt-wise and returns a
 * new dataframe with boolean values
 */
template <typename T>
DataFrame operator>(const DataFrame& lhs, T t) {
    if constexpr ((std::is_same<T, double>::value |
                   std::is_same<T, std::string>::value)) {
        DataFrame copy = deep_copy(lhs);
        for (const auto& x : copy.column_names)
            copy.columns[x.second]->is_greater_than(t, x.first);
        return copy;
    }
    std::string m("The function accepts only double and strings, in:\n");
    throw std::invalid_argument(m + __PRETTY_FUNCTION__);
}

template <typename T>
DataFrame& DataFrame::operator+=(const T& t) {
    for (auto& x : column_names) {
        Column new_col = *columns[x.second] + t;
        columns[x.second] = std::make_shared<Column>(new_col);
    }
    return *this;
}
#endif
