cmake_minimum_required (VERSION 3.5)

project (Dataframe VERSION 0.1)
set(LIBRARY_TARGET_NAME ${PROJECT_NAME})
SET (CMAKE_CXX_COMPILER             "/usr/bin/g++")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Produce compile_commands.json
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic)

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

add_executable(main main.cpp)
add_executable(my_test test/test.cpp)
add_executable(old_main test/old_main.cpp)
add_executable(mae_example test/mae_example.cpp)
add_executable(index_test test/index_test.cpp)
target_link_libraries(main Dataframe)
target_link_libraries(my_test Dataframe)
target_link_libraries(old_main Dataframe)
target_link_libraries(mae_example Dataframe)
target_link_libraries(index_test Dataframe)

file(COPY amits_example.csv DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

enable_testing()
add_test(NAME my_test
         COMMAND my_test)
