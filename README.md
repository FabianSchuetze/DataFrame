A pandas like dataframe in c++




Examples
--------
There are some examples what how the code can be used


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

Groupby
-------
The DataFrame can be grouped by values in one or multiple columns. 
```c++
p
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


