#include <string>
#include <vector>

class Column {
   public:
    Column();
    Column(const std::string, const std::vector<double>&);
    Column(const std::string, std::vector<double>&&);
    int size() { return data.size(); }
    double& operator[](std::size_t n) { return data[n]; }
    const double& operator[](std::size_t n) const { return data[n]; }

   private:
    std::vector<double> data;
    std::string name;
};
