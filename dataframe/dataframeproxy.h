#ifndef GUARD_DataFrameProxy_h
#define GUARD_DataFrameProxy_h
#include "dataframe.h"

class DataFrame::DataFrameProxy {
   public:
    friend class DataFrame;
    DataFrameProxy();
    DataFrameProxy(DataFrame&, const std::vector<std::string>&,
                   const std::string&);
    DataFrameProxy(DataFrame&, const std::vector<std::string>&,
                   const std::vector<std::string>&);
    DataFrameProxy& operator=(const DataFrameProxy&);
    DataFrameProxy& operator=(const std::vector<std::vector<double>>&);
    template <typename T>
    DataFrameProxy& operator=(const std::vector<T>&);
    ~DataFrameProxy() = default;
    friend std::ostream& operator<<(std::ostream&, const DataFrameProxy&);
    friend DataFrame operator+(const DataFrame::DataFrameProxy&,
                               const DataFrame::DataFrameProxy&);
    friend DataFrame operator+(const DataFrame&,
                               const DataFrame::DataFrameProxy&);

   private:
    DataFrame& theDataFrame;
    std::vector<std::string> idxNames;
    std::vector<std::string> colNames;
    // DO I REALLY NEED THIS FUNCTION???
    //template <typename T>
    //void add_or_replace(bool, int, const std::vector<T>&);
    void add_column(const std::shared_ptr<Column>&);
    void replace_column(int, const std::shared_ptr<Column>&);
    void check_size(size_t, std::string);
    void insert_column(const std::string&, std::shared_ptr<Column>&);
    template <typename T>
    void insert_column(const std::string&, const std::vector<T>&);
};

DataFrame operator+(const DataFrame::DataFrameProxy&,
                    const DataFrame::DataFrameProxy&);
std::ostream& operator<<(std::ostream&, const DataFrame::DataFrameProxy&);
#endif
