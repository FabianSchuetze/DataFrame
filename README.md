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



