cmake_minimum_required (VERSION 3.5)

project (Dataframe VERSION 0.1)
set(LIBRARY_TARGET_NAME ${PROJECT_NAME})
SET (CMAKE_CXX_COMPILER             "/usr/bin/g++")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Produce compile_commands.json
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(${LIBRARY_TARGET_NAME}_SRC
    dataframe/dataframe.cpp
    dataframe/column.cpp
    dataframe/print.cpp
    dataframe/operators.cpp
    dataframe/dataframeproxy.cpp
    dataframe/index.cpp
    dataframe/csv_reader.cpp
)

set(${LIBRARY_TARGET_NAME}_HDR
    dataframe/ColumIterator.h
    dataframe/dataframe.h
    dataframe/column.h
    dataframe/ConstColumnIterator.h
    dataframe/dataframeproxy.h
    dataframe/grouper.h
    dataframe/GroupbyFunctions.h
    dataframe/index.h
)

if (UNIX)
  add_library(${LIBRARY_TARGET_NAME} SHARED ${${LIBRARY_TARGET_NAME}_SRC})
endif(UNIX)
SET(CMAKE_CXX_FLAGS "-g -O0 --coverage -fprofile-arcs -ftest-coverage")
target_compile_options(${LIBRARY_TARGET_NAME} PUBLIC "--coverage" )

add_executable(main test/main.cpp)
add_executable(my_test test/test.cpp)
add_executable(test_groupby test/groupby.cpp)
add_executable(test_additions test/additions.cpp)
add_executable(test_iterator test/iterator.cpp)
add_executable(test_constructor test/constructor.cpp)
add_executable(test_csv test/csv.cpp)
add_executable(test_convenience test/convenience_functions.cpp)
add_executable(test_operator test/operator.cpp)
target_link_libraries(main Dataframe --coverage)
target_link_libraries(my_test Dataframe --coverage)
target_link_libraries(test_groupby Dataframe --coverage)
target_link_libraries(test_additions Dataframe --coverage)
target_link_libraries(test_iterator Dataframe --coverage)
target_link_libraries(test_constructor Dataframe --coverage)
target_link_libraries(test_csv Dataframe --coverage)
target_link_libraries(test_convenience Dataframe --coverage)
target_link_libraries(test_operator Dataframe --coverage)


file(COPY test/csv_example1.csv DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY test/csv_wronglength.csv DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY test/csv_wrongtype.csv DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY test/droprows.csv DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

enable_testing()
add_test(my_test my_test)
add_test(test_additions test_additions)
add_test(test_groupby test_groupby)
add_test(test_iterator test_iterator)
add_test(test_constructor test_constructor)
add_test(test_csv test_csv)
add_test(test_convenience test_convenience)
add_test(test_operator test_operator)
