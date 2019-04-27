A pandas like dataframe in c++

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
