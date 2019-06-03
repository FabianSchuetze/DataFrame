#ifndef GUARD_Column_h
#define GUARD_Column_h
#include <exception>
#include <iostream>
#include <string>
#include <variant>
#include <vector>
#include <algorithm>

class Column {
   private:
    std::variant<std::vector<double>, std::vector<std::string>> col;
    void replace_nan();

   public:
    friend std::ostream& operator<<(std::ostream&, const Column&);
    friend Column operator+(const Column&, double);
    friend Column operator+(const Column&, std::string);
    friend class DataFrame;
    Column();
    Column& operator+=(const Column& rhs);
    template <class T> Column(const std::vector<T>& t) { col = t; }
    template <class T> void push_back(const T);
    int size();
    template <class V> V operator[](int);
    template <typename V> typename std::vector<V>::iterator begin() {
        if (std::holds_alternative<std::vector<V>>(col))
            return std::get<std::vector<V>>(col).begin();
        else
            throw std::invalid_argument("not in here");
    }
    template <typename V> typename std::vector<V>::iterator end() {
        if (std::holds_alternative<std::vector<V>>(col))
            return std::get<std::vector<V>>(col).end();
        else
            throw std::invalid_argument("not in here");
    }
    void append_string(std::string&, int pos);
};

std::ostream& operator<<(std::ostream&, const Column&);
Column operator+(const Column&, double d);
Column operator+(const Column&, std::string s);
#endif
