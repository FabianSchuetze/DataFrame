A pandas like dataframe in c++




Examples
--------
There are some examples what how the code can be used

o
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







