#include <vector>
#include <string>

class Column{
    public:
        Column();
        Column(std::string, std::vector<double>&);
        Column(std::string, std::vector<double>&&);
    private:
        std::vector<double> data;
        std::string name;
        
};
