# DMTL

Data Mining Template Library (DMTL)

DMTL is an open-source, high-performance, generic data mining toolkit, written in C++. It provides a collection of generic algorithms and data structures for mining increasingly complex and informative patterns types such as: Itemsets, Sequences, Trees and Graphs.

DMTL utilizes a generic data mining approach, where all aspects of mining are controlled via a set of properties. The kind of pattern to be mined, the kind of mining approach to use, and the kind of data types and formats to mine over are all specified as a list of properties. This provides tremendous flexibility to customize the toolkit for various applications.

DMTL implements all algorithms following the vertical data mining approach. For itemsets, the implementation follows the Eclat approach (without diffsets). For sequences it follows the Spade approach. For trees it implements the SLEUTH framework, which allows one to mine embedded/induced and ordered/unordered trees. Finally, the graph mining framework uses a novel vertical approach.


BASIC INFORMAION
================
This software is an implementation of generic data mining algorithm. This particular implementation
consists of frequent pattern mining algorithm for itemset, sequence, tree, graph and multiset. But, 
due to the generic behavior of the algorithm, other patterns can also be added efficiently. Please 
refer to the conference and workshop papers under 'doc/papers' for further details.
This library uses C++ templates to implement the generic behavior, hence named Data Mining Template 
Library (DMTL).

INSTALL:
========
The library does not need to be installed. The code can be used by writing a driver program.
A driver program is a wrapper that utilizes different components of the library to perform 
the desired mining task.
Examples of driver programs are in the 'test' directory. In future we plan to bundle the 
code as a static library. 

To compile the driver programs : type 'make' in the test directory
To clean: type 'make clean' in test directory

PLATFORM:
=========
This program has been compiled using g++ 4.0 and tested on Linux and Mac OS X (10.4).

LICENSE TERMS
=============
Under GNU Public License terms. Please read the file COPYING

BUGS and QUESTIONS:
===================
If you find any bugs or have any questions, please contact:
Mohammed Zaki (zaki@cs.rpi.edu).

DIRECTORY CONTENTS:
===================
1. data: Some small data file to test the correctness and integrity of the library.
2. doc: Documentation of the library generated using doxygen.
3. src: Source code. The source code is further divided into 6 directories. There is a 
        directory each for the five patterns (itemset, sequence, tree, graph and multiset) and a 
        'common' directory that contains the code common to all the above patterns.
4. test: Test files to run mining algorithms for the following patterns (itemset, sequence, tree, graph, multiset)

