#include <vector>
#include <string>

class Column{
    public:
        Column();
        Column(const std::string, const std::vector<double>&);
        Column(const std::string, std::vector<double>&&);
    private:
        std::vector<double> data;
        std::string name;
        
};
