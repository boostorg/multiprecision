# Report on Implementation of `cpp_quad_float` and `cpp_double_float`
## Boost.Multiprecision | Google Summer of Code 2021
---

This document briefly descrbes the implementation of new `Boost.Multiprecision` backends `cpp_double_float` and `cpp_quad_float`, which resemble the double-float and quad-float types that are used to extend the precision of the underlying floating point types. The aim of this project was to set up a practical draft of a generic implementation of the mentioned types, along with compaitibility with the `Boost.Multiprecision number` wrapper, and writing exhaustive tests to prove correctness of the new types.

Working together with my mentors, I managed to accomplish the following:
- A generic implementation of a double-float class which _almost_ doubles the working precision of the underlying float
- A generic implementation of a quad-float class which _almost_ quadruples the working precision of the underlying float
- Compaitibility with `Boost.Multiprecision number` wrapper, and `std::numeric_limits<>`
- Exhaustive tests for constructors, arithmetic operators and comparision operators

## `cpp_double_float`
### Constructors
...
### Arithmetic operators
...
### `std::numeric_limits<>`
...
### Tests
...


## `cpp_quad_float`
### Constructors
...
### Arithmetic operators
...
### `std::numeric_limits<>`
...
### Tests
...