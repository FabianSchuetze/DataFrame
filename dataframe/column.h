#ifndef GUARD_Column_h
#define GUARD_Column_h
#include <algorithm>
#include <string>
#include <variant>
#include <vector>

class DataFrame;
class Column {
   public:
    friend Column operator+(const Column&, double);
    friend Column operator+(const Column&, const std::string&);
    friend class DataFrame;
    friend void append_missing_cols(DataFrame&, const DataFrame&);
    friend std::string::size_type width(const Column&, std::vector<std::string>&);

    Column();
    Column(const Column&, int);
    /**
     * @brief Copy constructor for a new column, given an existing colum and a
     * vecotr<int> indicating which position shall be copied
     */
    Column(const Column&, const std::vector<int>&);
    template <class T>
    Column(const std::vector<T>& t) { col = t; }
    /**
     * @brief similar to the overloaded assigment operator but it checks
     * equivalence
     */
    Column& plus(const Column&, const std::vector<std::pair<int, int>>&);
    template <class T>
    void push_back(const T);
    /**
     * @brief Returns the length of the column
     */
    const size_t size();
    const size_t size() const;
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

   private:
    template <typename T>
    void add_elements(std::vector<T>*, const std::vector<T>&,
                      const std::vector<std::pair<int, int>>&);
    std::variant<std::vector<double>, std::vector<std::string>> col;
    void replace_nan();
    void replace_nan(int);
    bool is_null(size_t);
};

Column operator+(const Column&, double d);
Column operator+(const Column&, const std::string& s);
#endif
