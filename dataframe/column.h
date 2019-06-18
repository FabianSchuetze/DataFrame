#ifndef GUARD_Column_h
#define GUARD_Column_h
#include <algorithm>
#include <string>
#include <variant>
#include <vector>
#include <deque>

class DataFrame;
class Column {
   public:
    friend Column operator+(const Column&, double);
    friend Column operator+(const Column&, const std::string&);
    friend class DataFrame;
    friend void append_missing_cols(DataFrame&, const DataFrame&);
    friend std::string::size_type width(const Column&, std::vector<std::string>&);
    template <typename T>
    friend DataFrame operator<(const DataFrame&, const T&);
    template <typename T>
    friend DataFrame operator>(const DataFrame&, const T&);

    Column(): col() {};
    //Column(const Column&, int); I THINK THIS CONSTRUCTOR IS NOT NEEDED?!?
    /**
     * @brief Copy constructor for a new column, given an existing colum and a
     * vector<int> indicating which position shall be copied
     */
    Column(const Column&, const std::deque<int>&);
    /**
     * @brief constructs a column form a existing vector. 
     * The vector must contains types of either double, string or boolean
     */
    template <class T>
    explicit Column(const std::vector<T>& t): col(t) {}
    /**
     * @brief similar to the overloaded assigment operator but it checks
     * equivalence
     */
    Column& plus(const Column&, const std::deque<std::pair<int, int>>&);
    /**
     * Pushes a value of type T into the column
     */
    void convert_and_push_back(const std::string&);
    template <class T>
    void push_back(const T);
    /**
     * @brief Returns the length of the column
     */
    size_t size();
    size_t size() const;
    /**
     * @breif Returns the type of the stored data as a string
     */
    std::string type_name();
    template <class T> T& get_value(int i) {
        if (std::holds_alternative<std::vector<T>>(col))
            return std::get<std::vector<T>>(col)[i];
        else {
            std::string s = "template func `get_value` instanted w wrong type";
            throw std::invalid_argument(s);
        }
    }
    void push_back_nan();
    std::string to_string(int) const;
    template <typename T>
    void copy_vector(const std::vector<T>*, const std::deque<int>&);
    void convert_bool_to_double();
    Column convert_bool_to_double(const Column&);
    void copy_row(int);

   private:
    template <typename T>
    void add_elements(std::vector<T>*, const std::vector<T>&,
                      const std::deque<std::pair<int, int>>&);
    std::variant<std::vector<double>, std::vector<std::string>,
                 std::vector<bool>> col;
    void replace_nan();
    void replace_nan(int);
    void is_null(std::vector<int>&);
    bool is_null(size_t);
    void add_to_double(const Column&,const  std::deque<std::pair<int, int>>&);
    void add_to_string(const Column&,const  std::deque<std::pair<int, int>>&);
    void is_smaller_than(const double&);
    void is_smaller_than(const std::string&);
    void is_greater_than(const double&);
    void is_greater_than(const std::string&);
    typedef std::vector<double> dvec;
    typedef std::vector<std::string> svec;
    typedef std::vector<bool> bvec;
};

Column operator+(const Column&, double d);
Column operator+(const Column&, const std::string& s);
#endif
