#include <deque>
#include <map>
#include <string>
#include <variant>
#include <vector>
class Index {
   public:
    typedef std::variant<int, std::string> ele;
    Index() : index_positions(), index_names(){};
    /**
     * Appends the input to the end of the index
     */
    void append_row(const std::deque<ele>&);
    /**
     * @brief Puts the input vector at the end of the existing index
     */
    template <typename T>
    void append_index_column(const std::vector<T>&);
    /**
     * @brief Drops the all index values at position pos inplace
     */
    void drop_index(size_t);
    /**
     * @brief Returns the corresponding index position given the input element
     */
    std::deque<int> find_index_position(const std::deque<ele>&);

   private:
    std::vector<std::deque<ele>> index_positions;
    std::map<std::deque<ele>, std::deque<int>> index_names;
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
    void append_value(const T& t, std::deque<ele>& set);
};
