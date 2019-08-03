

[![codecov](https://codecov.io/gh/FabianSchuetze/DataFrame/branch/master/graph/badge.svg?token=Pr0wisZZGU)](https://codecov.io/gh/FabianSchuetze/DataFrame)
[![Build Status](https://travis-ci.com/FabianSchuetze/DataFrame.svg?token=TFtgu8GKEKx7cZJZSt2F&branch=master)](https://travis-ci.com/FabianSchuetze/DataFrame)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/std/the-standard )

A pandas like dataframe in c++

Copy-on-Write
--------
A strong motivation to write this form of DataFrames, was Wes McKinney's blog post about (his new project) [Apache Arrow and the "10 Things I Hate About pandas" ](https://wesmckinney.com/blog/apache-arrow-pandas-internals/)
My Kinney says that keeping track of pandas memory is difficult as data can either be appropriated by pandas or the underlying
numpy arrays. He would instead use pointers to individuals columns whenever possible and create a unique column when the column is modified and more than one DataFrame references it. I implemented such a copy-on-write idiom here: 
```c++
vector<string> col = {"a", "b"};
vector<int> idx({1, 2});
vector<string> name({"column_a"});
DataFrame df1(Index(idx), name, col);
cout << df1.use_count("column_a") << endl; 
1
DataFrame df2 = df1;
cout << df1.use_count("column_a") << endl;
2
DataFrame::iterator<string> it = df1.begin<string>("column_a");
DataFrame::iterator<string>  e = df1.end<string>("column_a");
transform(it, e, it, [](auto& d) {return  d + "_modify";});
cout << df1.use_count("column_a") << endl;
1

```

Assignments
-----------
One of the things I like most about pandas is the ease with which one
can assign new columns. Thanks to [Scott Meyers](https://www.aristeia.com/)
``More Effective C++`` I could use the ProxyClass idiom to assign new columns
too:

```c++
vector<double> first({30});
vector<string> names = {"first_col"};
DataFrame df1 = DataFrame(names, first);
 -----------------
 |   | first_col |
 -----------------
 | 0 | 30.000000 |
 -----------------

vector<string> replace({"a"});
df1["first_col"] = replace;
 -----------------
 |   | first_col |
 -----------------
 | 0 | a         |
 -----------------

vector<vector<double>> expand_by_two({{-100, -200}, {400, 600}});
df1[vector<string>({"third_col", "fourth_col"})] = expand_by_two;
 --------------------------------------------
 |   | first_col | fourth_col | third_col   |
 --------------------------------------------
 | 0 | a         | 400.000000 | -100.000000 |
 --------------------------------------------
vector<bool> other({true, false});
DataFrame df2 = DataFrame(names, other);
df1["first_col"] = df2;
 --------------------------------------------
 |   | first_col | fourth_col | third_col   |
 --------------------------------------------
 | 0 | 1         | 400.000000 | -100.000000 |
 --------------------------------------------

```

STL Integration
---------------
Each column of the dataframe can be invoked as (bi-directional) iterators. The iterators
can thus cooperate with the STL algorithms, `std::algortihm`. Examples 
are:
```
c++
vector<double> data({1, 2, 3});
vector<string> idx({"a", "b", "c"});
DataFrame df = DataFrame(Index(idx), {"first_col"}, data);
DataFrame::iterator<double> it( df.begin<double>("first_col"));
DataFrame::iterator<double>  e(df.end<double>("first_col"));
auto res = find(it, e, 2); \\finds the value
transform(it, e, it, [](auto& d) {return sqrt(d);});
cout << df << endl;
 -----------------
 |   | first_col |
 -----------------
 | a | 1.000000  |
 | b | 1.414214  |
 | c | 1.732051  |
 -----------------
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
iterators. These iterators traverse in the direction of the DataFrame
index. They can be used in conjunction with the STL algorithms.
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
A DataFrame can be constructed from a CSV file. It is assumed that the
first two lines of the CSV file describe the schema of the table: The first line contains the column names and the second line the datatypes.
```c++
ifstream infile("csv_example1.csv");
DataFrame df1(infile);
 ---------------------------------------
 |            | first_col | second_col |
 ---------------------------------------
 | 2019/02/02 | 10.000000 | india      |
 ---------------------------------------
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

NA Values
---------
One can work with na values. NA values can be dropped or created when adding
dataframes with different indices
```c++
ifstream infile("contains_nan.csv");
DataFrame df1(infile);
cout << df1 << endl;
 ---------------------------------------
 |            | first_col | second_col |
 ---------------------------------------
 | 2019/02/02 | 10.000000 | nan        |
 | 2019/02/03 | nan       | 50.000000  |
 | 2019/02/04 | 10.000000 | -10.000000 |
 ---------------------------------------
 df1.dropna();
 ---------------------------------------
 |            | first_col | second_col |
 ---------------------------------------
 | 2019/02/04 | 10.000000 | -10.000000 |
 ---------------------------------------
```




