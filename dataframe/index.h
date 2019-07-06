#ifndef GUARD_index_h
#define GUARD_index_h
#include <deque>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <variant>
#include <vector>
class DataFrame;
class Index {
   public:
    friend class DataFrame;
    friend DataFrame deep_copy(const DataFrame& lhs);
    // NEED TO ADD DOUBLE!
    typedef std::variant<int, std::string> ele;
    typedef std::map<std::deque<ele>, std::deque<int>> index_map_type;
    Index() : index_positions(), index_names(){};
    template <typename T1, typename... T>
    Index(const std::vector<T1>&, const std::vector<T>&...);
    void append_index(const std::vector<std::deque<ele>>&);
    void append_index(const std::deque<ele>&);
    /** 
     * @brief Appends a vector of type int or string to the index
     */
    template <typename T>
    void append_index(const std::vector<T>&);
    template <typename T>
    void append_index(const T&);
    /**
     * Appends the input to the end of the index
     */
    void append_row(const std::deque<ele>&);
    /**
     * @brief Puts the input vector at the end of the existing index
     */
    template <typename T>
    void append_index_column(const std::vector<T>& inp);
    /**
     * @brief Drops the all index values at position pos inplace
     */
    void drop_index(size_t);
    /**
     * @brief Returns the corresponding index position given the input element
     */
    std::deque<int> find_index_position(const std::deque<ele>&);
    std::deque<int> find_index_position(const std::deque<ele>&) const;
    std::deque<int> find_index_position();
    std::deque<int> find_index_position() const;
    std::vector<std::deque<ele>> get_index_names();
    std::vector<std::deque<ele>> get_index_names() const;
    std::vector<std::string> get_index_as_string() const;
    std::vector<std::string> get_index_as_string();
    int size() { return index_positions.size(); }
    int size() const { return index_positions.size(); }
    std::set<std::deque<ele>> unique_elements();
    std::set<std::deque<ele>> unique_elements() const;

   private:
    std::vector<std::deque<ele>> index_positions;
    index_map_type index_names;
    /**
     * Given the input deque, the function inplace removes inplace the element
     * at the desired position
     */
    void remove_value(size_t pos, std::deque<ele>& t);
    /**
     * Given the input deque, the function appends inplace the element at the
     * desired position.
     */
    template <typename T>
    void expand_value(const T& t, std::deque<ele>& set) {
        set.push_back(t);
    }
    template <typename T1>
    void expand_index(const std::vector<T1>&);
    template <typename T1, typename... T>
    void expand_index(const std::vector<T1>&, const std::vector<T>&...);
};

template <typename T>
void check_values() {
    static_assert(
        std::is_same<T, int>::value || std::is_same<T, std::string>::value,
        "Index restricted to int or string");
}

template <typename T>
void Index::append_index(const T& t) {
    std::deque<ele> s{t};
    append_index(s);
}

template <typename T>
void Index::append_index(const std::vector<T>& inp) {
    check_values<T>();
    for (const T& t : inp) append_index(t);
}

template <typename T>
void Index::append_index_column(const std::vector<T>& inp) {
    if (inp.size() != index_positions.size()) {
        std::string m("The input column is of different size, in:\n");
        throw std::invalid_argument(m + __PRETTY_FUNCTION__);
    }
    std::map<std::deque<ele>, std::deque<int>> new_index_names;
    int i = 0;
    for (auto& tmp : index_positions) {
        expand_value(inp[i], tmp);
        new_index_names[tmp].push_back(i++);
    }
    index_names = new_index_names;
}

template <typename T1>
void Index::expand_index(const std::vector<T1>& v1) {
    append_index_column<T1>(v1);
}

template <typename T1, typename... T>
void Index::expand_index(const std::vector<T1>& v1,
                         const std::vector<T>&... v2) {
    append_index_column<T1>(v1);
    expand_index<T...>(v2...);
}

template <typename T1, typename... T>
Index::Index(const std::vector<T1>& v1, const std::vector<T>&... v2)
    : index_positions(), index_names() {
    append_index(v1);
    if constexpr (sizeof...(T) > 0) expand_index<T...>(v2...);
}
#endif
