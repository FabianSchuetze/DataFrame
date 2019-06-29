#include "dataframe.h"
#include <sstream>
using std::string;
using std::vector;
using std::make_shared;
template <typename T>
void DataFrame::initialize_column(const string& s) {
    Column col = Column(vector<T>());
    columns[find_column_position(s)] = make_shared<Column>(col);
}

void DataFrame::initialize_column(std::ifstream& file,
                                  const vector<string>& cols) {
    columns.resize(cols.size());
    std::string line, name;
    std::getline(file, line);
    std::istringstream names(line);
    std::getline(names, name, ',');  // pass the index column
    size_t i = 0;
    while (std::getline(names, name, ',')) {
        string colName = cols[i++];
        if (name == "double")
            initialize_column<double>(colName);
        else if (name == "string")
            initialize_column<string>(colName);
        else if (name == "bool")
            initialize_column<bool>(colName);
        else
            throw std::invalid_argument("Input type: " + name +
                                        " incompatible");
    }
}

void DataFrame::insert_data(std::ifstream& file, const vector<string>& cols) {
    std::string line, name;
    while (std::getline(file, line)) {
        std::istringstream names(line);
        std::getline(names, name, ',');
        index.append_index(name);
        for (const string& col : cols) {
            std::getline(names, name, ',');
            columns[find_column_position(col)]->convert_and_push_back(name);
        }
    }
}

vector<string> DataFrame::create_column_names(std::ifstream& file) {
    std::string line, name;
    std::getline(file, line);
    size_t i = 0;
    vector<string> cols;
    std::istringstream names(line);
    std::getline(names, name, ',');  // pass the index column
    while (std::getline(names, name, ',')) {
        cols.push_back(name);
        try {
            column_names.at(name);
            throw std::invalid_argument("Duplicate column name " + name);
        } catch (std::out_of_range& e) {
            column_names[name] = i++;
        }
    }
    return cols;
}
