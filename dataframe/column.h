#ifndef GUARD_Column_h
#define GUARD_Column_h
#include <algorithm>
#include <string>
#include <variant>
#include <vector>

class DataFrame;
class Column {
   public:
    friend std::ostream& operator<<(std::ostream&, const Column&);
    friend Column operator+(const Column&, double);
    friend Column operator+(const Column&, const std::string&);
    friend class DataFrame;
    friend void append_missing_cols(DataFrame&, const DataFrame&);
    friend std::string::size_type width(const Column&, std::vector<std::string>&);

    Column();
    Column(const Column&, int);
    Column(const Column&, const std::vector<int>&);
    template <class T>
    Column(const std::vector<T>& t) { col = t; }
    Column& operator+=(const Column& rhs);
    Column& plus(const Column&, const std::vector<std::pair<int, int>>&);
    template <class T>
    void push_back(const T);
    const size_t size();
    const size_t size() const;
    template <class T> T& get_value(int i) {
        if (std::holds_alternative<std::vector<T>>(col))
            return std::get<std::vector<T>>(col)[i];
        else
            throw std::invalid_argument("not in here");
    }
    void append_string(std::string&, int pos);
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

std::ostream& operator<<(std::ostream&, const Column&);
Column operator+(const Column&, double d);
Column operator+(const Column&, const std::string& s);
#endif
