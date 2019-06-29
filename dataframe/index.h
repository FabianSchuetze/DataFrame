#ifndef GUARD_index_h
#define GUARD_index_h
#include <deque>
#include <map>
#include <string>
#include <variant>
#include <vector>
#include <set>
class DataFrame;
class Index {
   public:
    friend class DataFrame;
    friend DataFrame deep_copy(const DataFrame& lhs);
    typedef std::variant<int, std::string> ele;
    typedef std::map<std::deque<ele>, std::deque<int>> index_map_type;
    Index() : index_positions(), index_names(){};
    void append_index(const std::vector<std::deque<ele>>&);
    void append_index(const std::deque<ele>&);
    //template <typename T>
    void append_index(const std::vector<std::string>&);
    void append_index(const std::vector<int>&);
    void append_index(const int&);
    void append_index(const std::string&);
    /**
     * Appends the input to the end of the index
     */
    void append_row(const std::deque<ele>&);
    /**
     * @brief Puts the input vector at the end of the existing index
     */
    template <typename T>
    void append_index_column(const std::vector<T>& inp) {
        if (inp.size() != index_positions.size())
            throw std::invalid_argument("Cant do it in " + __PRETTY_FUNCTION__);
        std::map<std::deque<ele>, std::deque<int>> new_index_names;
        int i = 0;
        for (auto& tmp : index_positions) {
            append_value(inp[i], tmp);
            new_index_names[tmp].push_back(i++);
        }
        index_names = new_index_names;
    }
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
    int size() {return index_positions.size();}
    int size()const {return index_positions.size();}
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
    void append_value(const T& t, std::deque<ele>& set) {
        set.push_back(t);
    }
};
#endif
