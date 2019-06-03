#ifndef GUARD_dataframe_h
#define GUARD_dataframe_h
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <cmath>
#include "column.h"
#include <algorithm>

class DataFrame {
   public:
    class DataFrameProxy;
    DataFrame();
    template <typename T>
    DataFrame(const std::vector<std::string>&, 
              const std::vector<std::vector<T>>&);
    DataFrame(const DataFrameProxy&);
    DataFrame& operator=(const DataFrame&);

    friend class DataFrameProxy;
    friend std::ostream& operator<<(std::ostream&, const DataFrame&);
    friend std::ostream& operator<<(std::ostream&, const DataFrameProxy&);
    friend DataFrame operator+(const DataFrame& lhs, const DataFrame& rhs);
    // according to item 46, we need to define it here
    template <typename T> friend DataFrame operator+(const DataFrame& lhs, 
                                                     const T& t) {
        DataFrame new_df = lhs;
        for (auto& x : lhs.column_names) {
            Column new_col = *new_df.columns[x.second] + t;
            new_df.columns[x.second] = std::make_shared<Column>(new_col);
        }
        return new_df;
    }

    class DataFrameProxy {
       public:
        friend class DataFrame;
        DataFrameProxy();
        ~DataFrameProxy() = default;
        DataFrameProxy(DataFrame&, const std::string&);
        DataFrameProxy(DataFrame&, const std::vector<std::string>&);
        DataFrameProxy& operator=(const DataFrameProxy&);
        DataFrameProxy& operator=(const std::vector<std::vector<double>>&);
        template <typename T> DataFrameProxy& operator=(const std::vector<T>&);
        friend std::ostream& operator<<(std::ostream&, const DataFrameProxy&);

       private:
        DataFrame& theDataFrame;
        std::vector<std::string> colNames;
        template <typename T>
        void add_or_replace(bool, int, const std::vector<T>&);
        void add_or_replace(bool, int, const std::shared_ptr<Column>&);
        void check_column_size(size_t);
        template <typename T> 
        void insert_column(const std::string&, const std::vector<T>&);
    };
    DataFrame& operator+=(const DataFrame& rhs);
    template <class T> typename std::vector<T>::iterator begin(std::string);
    template <class T> typename std::vector<T>::iterator end(std::string);
    //template <class T> typename std::vector<T>::const_iterator begin(std::string);
    template <class T> typename std::vector<T>::const_iterator end(std::string);
    //what shall I do with the constant operator?
    DataFrameProxy operator[](const std::string& col_name);
    DataFrameProxy operator[](const std::vector<std::string>& col_name);
    const std::pair<int, int> size() const;
    // THis is a strange function, can I const qualify it?
    const int use_count(std::string);

   private:
    std::vector<std::shared_ptr<Column>> columns;
    std::map<std::string, int> column_names;
    void make_unique_if(const std::string&);
    template <class T> void 
    add_elements(std::vector<T>& inp, const DataFrame& other, 
                 int pos, int otherPos) {
        std::transform(columns[pos]->begin<T>(), columns[pos]->end<T>(),
                       other.columns[otherPos]->begin<T>(), 
                       std::back_inserter(inp), std::plus<T>());
    }
};

template <typename T>
DataFrame operator+(const DataFrame&, const T&);
DataFrame operator+(const DataFrame&, const DataFrame&);
std::ostream& operator<<(std::ostream&, const DataFrame&);
std::ostream& operator<<(std::ostream&, const DataFrame::DataFrameProxy&);
#endif
