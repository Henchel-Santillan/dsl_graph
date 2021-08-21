# dsl_graph

**dsl_graph** is an extension of the DSL and contains tree and graph data structures. The containers have been developed to be stl-incompatible (meaning object creation and memory allocation have not been separated, and iterators are not supported, among other incompatibilities). 

Pre-C++11 owning raw pointers are used in the implementations for simplicity.  

## Toolchain
* Environment: `cygwin64 v3.2.0`
* CMake: `v3.19.2`
* Compiler: `gcc 10`
* Debugger: `gdb v9.2`
* CXX_Standard: `C++20`

## Supported Containers
* `binary_tree`
* `binary_search_tree`
* `avl_tree`
* `max_heap`
* `digraph`

## TODO
* Increased container support
* `smart pointer` usage (e.g., `std::unique_ptr` and `std::shared_ptr`)
* An extension to the dsl library supporting a select few containers that are concurrent
* Unit Testing with `googletest`
