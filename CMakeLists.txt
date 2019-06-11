cmake_minimum_required (VERSION 3.5)

project (Dataframe VERSION 0.1)
set(LIBRARY_TARGET_NAME ${PROJECT_NAME})

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
)

set(${LIBRARY_TARGET_NAME}_HDR
    dataframe/ColumIterator.h
    dataframe/dataframe.h
    dataframe/column.h
    dataframe/ConstColumnIterator.h
    dataframe/dataframeproxy.h
)

if (UNIX)
  add_library(${LIBRARY_TARGET_NAME} SHARED ${${LIBRARY_TARGET_NAME}_SRC})
endif(UNIX)

add_executable(main main.cpp)
add_executable(my_test test/test.cpp)
target_link_libraries(main Dataframe)
target_link_libraries(my_test Dataframe)

file(COPY forecast2.csv DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY amits_example.csv DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

enable_testing()
add_test(NAME my_test
         COMMAND my_test)
