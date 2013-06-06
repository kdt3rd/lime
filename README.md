lime
====

Re-implementation of lemon (one of the tools maintained as part of sqlite)
in C++.

Similar to bison, but the syntax and construction mechanisms mean that
it is easy to create code that is thread-safe, and handles errors more
cleanly - properly freeing memory and exception safe.
