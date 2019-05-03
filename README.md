A pandas like dataframe in c++

TO DO
------
The pandas dataframe organized its rows and columns with a map. The keys are
the indices and the values are inters, saying where the data is stored. I would
like to try the same.

Additionally, the dataframe is stores in block, one for each datatype. I would
also like to the data same. How can this be done?

Furthermore: When I select a column, is pandas returning a new object or a
reference to the old oject? I want to return a reference, I guess.

I need to create the data once and then only store a pointer to the first
element!

Questions
---------
* Do we need a destructor? If so, the rule of three tells us that we should
    implement a copy constructor and an assignment operator as well
* When is it in general useful to work with references and pointers? I doubt
    the copy is idiomatic for good c++
* How can I organize the code well? Can I put have one source file for the
    constructors, one for the opertors, etc?
* So far, I return vectors<double> with the assignment operators. However, I
    should return another dataframe as well


