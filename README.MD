C++ Link State Algorithm Library
============================

An abstract implementation of the Link State Routing algorithm.

Features 
---
- A method for automatically cleaning up unreachable nodes
- Customisable node identifier types, distance types, and edge/node limits (through templates)

Dependencies
-----
Since this library was originally used for an embedded project, the included makefile is built for [BMPTK](http://github.com/wovo/bmptk).
However, the library can also be compiled without BMPTK, or just directly included, since it's header only. 


Installation/usage
-----
- Download the library `git clone https://github.com/Niels-Post/cpp_link_state_algorithm`
- Include Makefile.inc from your project
    **OR**
- Include *calculator.h* in your project

Building with BMPTK
----
- Make sure to set the TARGET before including *Makefile.inc*
- Include *Makefile.inc*


Building without BMPTK
----
The included makefile only sets some variables, but doesn't do any compiling itself. When building without BMPTK, some variables should be interpreted properly after including.
- HEADERS: all .hpp header files
- SEARCH: the include path for header files of this library 


