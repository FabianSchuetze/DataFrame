#ifndef GUARD_dataframe_h
#define GUARD_dataframe_h
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <cmath>
#include "column.h"


class DataFrame {
   public:
    class DataFrameProxy;
    DataFrame();
    DataFrame(std::vector<std::string>, std::vector<std::vector<double>>);
    DataFrame(const DataFrameProxy);

    friend class DataFrameProxy;
    friend std::ostream& operator<<(std::ostream&, const DataFrame&);
    friend std::ostream& operator<<(std::ostream&, const DataFrameProxy&);
    friend DataFrame operator+(const DataFrame& lhs, const DataFrame& rhs);

    class DataFrameProxy {
       public:
        friend class DataFrame;
        DataFrameProxy();
        ~DataFrameProxy() = default;
        DataFrameProxy(DataFrame&, std::string);
        DataFrameProxy(DataFrame&, std::vector<std::string>);
        DataFrameProxy& operator=(const DataFrameProxy&);
        DataFrameProxy& operator=(std::vector<std::vector<double>>&);
        DataFrameProxy& operator=(std::vector<double>&);
        DataFrameProxy& operator=(std::vector<string>&);
        friend std::ostream& operator<<(std::ostream&, const DataFrameProxy&);

       private:
        void add_or_replace(bool, int, vector<double>&);
        void add_or_replace(bool, int, vector<string>&);
        void add_or_replace(bool, int, std::shared_ptr<Column>);
        DataFrame& theDataFrame;
        std::vector<std::string> colNames;
    };
    DataFrame& operator+=(const DataFrame& rhs); //need to think about copy on write!!!
    DataFrameProxy operator[](std::string col_name);
    DataFrameProxy operator[](std::vector<std::string> col_name);
    const std::pair<int, int> size() const;
    const int use_count(std::string);

   private:
    std::vector<std::shared_ptr<Column>> columns;
    std::map<std::string, int> column_names;
};

DataFrame operator+(const DataFrame& lhs, const DataFrame& rhs);
std::ostream& operator<<(std::ostream&, const DataFrame&);
std::ostream& operator<<(std::ostream&, const DataFrame::DataFrameProxy&);
#endif
