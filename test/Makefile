### GLOBALS
CC	   	= g++-mp-4.8
CFLAGS	   	= -g -O3 -Wall
INCLUDE-PATH 	= -I. -I../src/common -I../src/itemset -I../src/multiset \
		  -I../src/sequence -I../src/tree -I../src/graph
INCLUDES-COMMON = ../src/common/*.h
INCLUDES-ISET = ../src/itemset/*.h
INCLUDES-MSET = ../src/multiset/*.h
INCLUDES-SEQ = ../src/sequence/*.h
INCLUDES-TREE = ../src/tree/*.h
INCLUDES-GRAPH = ../src/graph/*.h

### TARGETS
MEMORY-BASED  = itemset_test multiset_test sequence_test tree_test graph_test

FILE-BASED    =  itemset_test_file sequence_test_file tree_test_file \
                graph_test_file

all: memory-based file-based

memory-based: $(MEMORY-BASED)

file-based: $(FILE-BASED)

default: all

clean: 
	rm -f $(MEMORY-BASED) $(FILE-BASED) *.o

### RULES
.SUFFIXES: .cpp

.cpp: 
	$(CC) $(CFLAGS) $(INCLUDE-PATH) $< -o $@

### DEPENDENCIES
# target: headers
itemset_test:  $(INCLUDES-ISET) $(INCLUDES-COMMON)
itemset_test_file:  $(INCLUDES-ISET) $(INCLUDES-COMMON)
multiset_test:  $(INCLUDES-MSET) $(INCLUDES-COMMON)
sequence_test:  $(INCLUDES-SEQ) $(INCLUDES-COMMON)
sequence_test_file:  $(INCLUDES-SEQ) $(INCLUDES-COMMON)
tree_test:  $(INCLUDES-TREE) $(INCLUDES-COMMON)
tree_test_file:  $(INCLUDES-TREE) $(INCLUDES-COMMON)
graph_test:  $(INCLUDES-GRAPH) $(INCLUDES-COMMON)
graph_test_file:  $(INCLUDES-GRAPH) $(INCLUDES-COMMON)
