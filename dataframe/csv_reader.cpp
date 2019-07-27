#include <sstream>
#include "dataframe.h"
using std::istringstream;
using std::make_shared;
using std::string;
using std::vector;

istringstream get_stringstream(std::ifstream& file) {
    string line;
    std::getline(file, line);
    std::istringstream names(line);
    if (!(names.rdbuf()->in_avail())) {
        string m("Empty line while constructing col of df, in:\n");
        throw std::invalid_argument(m + __PRETTY_FUNCTION__);
    }
    return names;
}

template <typename T>
void DataFrame::initialize_column(const string& s) {
    Column col = Column(vector<T>());
    columns[find_column_position(s)] = make_shared<Column>(col);
}

void DataFrame::initialize_column(const string& type, const string& name) {
    if (type == "double")
        initialize_column<double>(name);
    else if (type == "string")
        initialize_column<string>(name);
    else if (type == "bool")
        initialize_column<bool>(name);
    else {
        std::string m("Wrong column type: " + type + " for: " + name +" in:\n");
        throw std::invalid_argument(m + __PRETTY_FUNCTION__);
    }
}
void DataFrame::initialize_column(std::ifstream& file,
                                  const vector<string>& cols) {
    columns.resize(cols.size());
    istringstream names = get_stringstream(file);
    string name;
    std::getline(names, name, ',');  // pass the index column
    size_t i = 0;
    while (std::getline(names, name, ','))
        initialize_column(name, cols[i++]);
}

void DataFrame::insert_data(std::ifstream& file, const vector<string>& cols) {
    string line, name;
    while (std::getline(file, line)) {
        istringstream names(line);
        std::getline(names, name, ',');
        index.append_index(name);
        for (const string& col : cols) {
            std::getline(names, name, ',');
            columns[find_column_position(col)]->convert_and_push_back(name);
        }
    }
}

void DataFrame::create_column_names(const std::string& s, const char* fun) {
    try {
        column_names.at(s);
        std::string m("Duplicate column name " + s + "in:\n");
        throw std::invalid_argument(m + fun);
    } catch (std::out_of_range& e) {
        column_names[s] = column_names.size();
    }
}

vector<string> DataFrame::create_column_names(std::ifstream& file) {
    istringstream names = get_stringstream(file);
    string name;
    vector<string> cols;
    std::getline(names, name, ',');
    while (std::getline(names, name, ',')) {
        cols.push_back(name);
        create_column_names(name, __PRETTY_FUNCTION__);
    }
    return cols;
}

DataFrame::DataFrame(std::ifstream& file) : columns(), column_names() {
    vector<string> colNames = create_column_names(file);
    initialize_column(file, colNames);
    insert_data(file, colNames);
    assert_same_column_length(__PRETTY_FUNCTION__);
}
