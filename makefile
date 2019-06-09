CC=g++
CFLAGS= -I.
CXXFLAGS=-std=c++17
DEPS = dataframe/dataframe.h dataframe/column.h dataframe/dataframeproxy.h dataframe/ColumnIterator.h dataframe/ColumnIterator.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

out: main.o dataframe/operators.o dataframe/column.o dataframe/dataframe.o dataframe/dataframeproxy.o dataframe/print.o
	$(CC) -o out main.o dataframe/operators.o dataframe/column.o dataframe/dataframe.o dataframe/dataframeproxy.o dataframe/print.o
