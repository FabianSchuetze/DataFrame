

[![codecov](https://codecov.io/gh/FabianSchuetze/DataFrame/branch/master/graph/badge.svg?token=Pr0wisZZGU)](https://codecov.io/gh/FabianSchuetze/DataFrame)

[![Build Status](https://travis-ci.com/FabianSchuetze/DataFrame.svg?token=TFtgu8GKEKx7cZJZSt2F&branch=master)](https://travis-ci.com/FabianSchuetze/DataFrame)

A pandas like dataframe in c++

Examples
--------
There are some examples what how the code can be used

Copy-on-Write
--------
A strong motivation to write this form of dataframes, was Wes McKinney's 
blog post about (his new project)
 [Apache Arrow and the "10 Things I Hate About pandas" ](https://wesmckinney.com/blog/apache-arrow-pandas-internals/)
 In it, he says that keeping track of pandas memory is difficult as memory can either belong to pandas or underlying 
 numpy arrays and that Apache Arrow should referece difference column when possible but when
 a column with mutiple references is modified, it should be copied first. I tried to
 implement such copy-on-write ideom as well. 
```c++
vector<string> col = {"a", "b"};
vector<int> idx({1, 2});
vector<string> name({"column_a"});
DataFrame df1(Index(idx), name, col);
cout << df1.use_count("column_a") << endl; \\returns 1
DataFrame df2 = df1;
cout << df1.use_count("column_a") << endl; \\returns 2
DataFrame::iterator<string> it = df1.begin<string>("column_a");
DataFrame::iterator<string>  e = df1.end<string>("column_a");
transform(it, e, it, [](auto& d) {return  d + "_modify";});
cout << df1.use_count("column_a") << endl; \\returns 1 again

```

Addition
--------
Addition relates the different column and indices of both DataFrames
automatically:
```c++

DataFrame dfa(Index(vector<int>({1, 1, 3})), {"colOne"},
              vector<double>({-6, 0, 10}));
DataFrame dfb(Index(vector<int>({1})), {"colOne"}, vector<double>({8}));
DataFrame sum = dfa + dfb;
std::cout << sum << std::endl;
 ----------------
 |   | colOne   |
 ----------------
 | 1 | 2.000000 |
 | 1 | 8.000000 |
 | 3 | nan      |
 ----------------
```

Assignments
-----------
One of the things I like most about pandas is the ease with which one
can assign new columns. Thanks to [Scott Meyers](https://www.aristeia.com/) 
``More Effective C++` I could use the ProxyClass ideom to assign new columns
too:

```c++
vector<double> first({30, 40});
vector<string> names = {"first_col"};
DataFrame df1 = DataFrame(names, first1);
vector<string> replace({"a", "b"});
df1["first_col"] = replace; \\replace the column
vector<vector<double>> expand_by_two({{-100, -200}, {400, 600}});
df1[vector<string>({"third_col", "fourth_col"})] = expand_by_two; \\expands the df
vector<bool> other({true, false});
DataFrame df2 = DataFrame(names, other);
df1["first_col"] = df2; \\ use a different df to replace cols
```

SummaryStatistics
-----------------
One can calculate various summary statistics across columns.
```c++
vector<string> key1({"a", "a", "b", "b", "a"});
vector<double> data1({1, 1, 3, 4, 5});
vector<double> data2({11, 12, 13, 14, 15});
DataFrame df({"key1", "data1", "data2"}, key1, data1, data2);
head t;
Statistic *p = &t;
cout << df.summarize(p) << endl;
 --------------------------------------
 |      | data1    | data2     | key1 |
 --------------------------------------
 | head | 1.000000 | 11.000000 | a    |
 --------------------------------------

 ---------------------------
 |             | data2     |
 ---------------------------
 | a, 1.000000 | 11.500000 |
 | a, 5.000000 | 15.000000 |
 | b, 3.000000 | 13.000000 |
 | b, 4.000000 | 14.000000 |
 ---------------------------
```

Groupby
-------
The DataFrame can be grouped by values in one or multiple columns. 
```c++

vector<string> key1({"a", "a", "b", "b", "a"});
vector<double> data1({1, 1, 3, 4, 5});
vector<double> data2({11, 12, 13, 14, 15});
DataFrame df({"key1", "data1", "data2"}, key1, data1, data2);
mean t;
Statistic *p = &t;
DataFrame::Grouper<string, double> grouper =
df.groupby<string, double>(df.cbegin<string>("key1"),
                           df.cbegin<double>("data1"));
cout << grouper.summarize(p) << endl;
 ---------------------------
 |             | data2     |
 ---------------------------
 | a, 1.000000 | 11.500000 |
 | a, 5.000000 | 15.000000 |
 | b, 3.000000 | 13.000000 |
 | b, 4.000000 | 14.000000 |
 ---------------------------
```

Iterators
---------
The columns of the iterators provide const and non_const (bi-directional)
iterators. These iterators traverse in the direction of the dataframe
index. They can be used in conjunction with the STL algrotihms.
```c++
vector<string> key1({"a", "a", "b", "b", "f"});
vector<double> data1({1, 2, 3, 4, 5});
std::vector<std::string> idx = {"1", "2", "4", "5", "6"};
DataFrame df(Index(idx), {"key1", "data1"}, key1, data1);
DataFrame::iterator<string> it(df.begin<string>("key1"));
DataFrame::iterator<string> e(df.end<string>("key1"));
vector<string>res(it, e);
std::reverse_copy(it, e, it);
```

CSV Reader
---------
A DataFrame can be constructed from a csv file. Is is assumed that the 
first two lines of the csv file describe the schema of the table: The first 
line contains the column names and the second line the datatypes.
```c++
ifstream infile("csv_example1.csv");
DataFrame df1(infile);
 ---------------------------------------
 |            | first_col | second_col |
 ---------------------------------------
 | 2019/02/02 | 10.000000 | india      |
 ---------------------------------------
```

NA Values
---------
One can work with na values. NA values can be dropped or created when adding
dataframes with different indices





