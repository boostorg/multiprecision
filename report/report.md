# Report on Implementation of `cpp_quad_float` and `cpp_double_float`
## Boost.Multiprecision | Google Summer of Code 2021
---

This document briefly descrbes the implementation of new `Boost.Multiprecision` backends `cpp_double_fp_backend` and `cpp_quad_fp_backend`, which resemble the double-float and quad-float types that are used to extend the precision of the underlying floating point types. The aim of this project was to set up a practical draft of a generic implementation of the mentioned types, along with compaitibility with the `Boost.Multiprecision number` wrapper, and writing exhaustive tests to prove correctness of the new types.

The mentioned backends implement the underlying arithmetic operators and algorithms on a lower level, while maintaining compaitibility with Boost.Multiprecision `number<>` wrapper (through the required [`eval_*()`](https://www.boost.org/doc/libs/1_77_0/libs/multiprecision/test/skeleton_backend.hpp) routines) which allows the end-user and all Boost libraries to interact with the class on a higher level. Finally, the `std::numeric_limits<>` specialization of the classes, along with `number<>` wrapper from Boost.Multiprecision allows easy integration in the rich library of generic Boost algorithms and tests. Both of the implementations were rigorously tested for correctness, and some unresolved issues remain which are planned to be tackled post-GSoC.

## The double-float implementation
The design philosophy behind the double-float implementation was to write a generic template class called `cpp_double_fp_backend<>` which could theoretically double the precision of any IEEE-compaitible floating-point type, and to wrap it up using Boost.Multiprecision `number<>` wrapper. These wrappers are designed to mimic the native-float types, through extensive list of constructors and operator overloading:
```
using boost::multiprecision;
cpp_double_float a = 22.0, b = 7, c = "3.1415926535897932384626433832795";
std::cout << "pi - 22/7 == " << c - a / b << std::endl;
```

### `cpp_double_fp_backend`
`cpp_double_fp_backend<>` is the backend type behind this implementation of double-float type. It is a generic template class which consitutes of a pair of two non-overlapping native floating point types, the sum of which equals the represented number. Various algorithms are then used to perform basic arithmetic operations such as addition and multiplication, on top of which more complicated algorithms are built.
It has been exhaustively tested with 4-byte `float`, 8-byte `double`, 10-byte `long double`, and 16-byte `float128`.

#### Boost.Multiprecision `number<>` wrapper
The `number<>` wrapper wraps the `cpp_double_fp_backend<>` class to be easily usable by the end-user and compaitible with the rest of Boost library, which includes a rich set of mathematical algorithms, generic tests of correctness, and even overloaded constructors and operators for the `cpp_double_fp_backend<>` class. The available `number<>` wrappers for this class are:
```
using cpp_double_float       = number<backends::cpp_double_fp_backend<float>>;
using cpp_double_double      = number<backends::cpp_double_fp_backend<double>>;
using cpp_double_long_double = number<backends::cpp_double_fp_backend<long double>>;
using cpp_double_float128    = number<backends::cpp_double_fp_backend<float128>>;
```
_// TODO List relevant code_

#### Constructors
Most of the constructors are straighforward, except the constructors from arithmetic types which contain more information than a single underlying native floating-point type can hold. In those cases, special care needs to be taken to extract out maximum information.
_// TODO List relevant code_

#### Arithmetic operators
The arithmetic operatory algorithms were primarily adaptations of David Bailey's [QD package](https://www.davidhbailey.com/dhbsoftware/qd-2.3.23.tar.gz).
_// TODO List relevant code_

#### `std::numeric_limits<>` specialization
There was [significant discussion](https://github.com/BoostGSoC21/multiprecision/issues/21) regarding the correct values of `numeric_limits::digits` and `numeric_limits::digits10` for this class. The discussion was surrounded around the fact that in `cpp_double_fp_backend` a few bits of precision are lost during arithmetic operations, and this was needed to be reflected in the `std::numeric_limits<>` specialization for the class. The final [consensus](https://github.com/BoostGSoC21/multiprecision/issues/21#issuecomment-892638810) was that `numeric_limits::epsilon()` will be used to reflect the precision provided by the type instead of `numeric_limits::digits`.
_// TODO List relevant code_

#### Tests
The double-float implementation was exhaustively tested through two sets of tests:
- **Homegrown Tests**: These included stress-tests for arithmetic operators, constructors, comparison operators, and decomposition. The focus of these tests was to ensure accuracy and correctness.
- **Boost generic Tests**: These included tests for arithmetic robustness, computation of important mathematical functions including trigonometric and exponential functions.

_// TODO List relevant code_
#### Issues
_// TODO List known open and resolved issues_

### Performance
`cpp_double_fp_backend` was tested against `cpp_bin_float` of similar precision. The following table summarizes the relative performance of the types (each operation was performed 10,000 times):

| Operator                      | cpp_bin_float        | cpp_double_fp_backend |
| ----------------------------- | -------------------- | --------------------- |
| *                             | 11.114 (0.362158s)   | ***1** (0.0325857s)   |
| *(int)                        | 7.27636 (0.242081s)  | ***1** (0.0332695s)   |
| *(unsigned long long)         | 4.3046 (0.306152s)   | ***1** (0.0711219s)   |
| *=(unsigned long long)        | 7.6058 (0.303511s)   | ***1** (0.0399052s)   |
| +                             | 5.60299 (0.377083s)  | ***1** (0.0673002s)   |
| +(int)                        | 8.00336 (0.440066s)  | ***1** (0.0549851s)   |
| +(unsigned long long)         | 7.48374 (0.466146s)  | ***1** (0.0622879s)   |
| +=(unsigned long long)        | 9.05229 (0.440637s)  | ***1** (0.0486769s)   |
| -                             | 7.12818 (0.39518s)   | ***1** (0.0554391s)   |
| -(int)                        | 7.69794 (0.447368s)  | ***1** (0.0581152s)   |
| -(unsigned long long)         | 6.90328 (0.455248s)  | ***1** (0.0659466s)   |
| -=(unsigned long long)        | 8.97643 (0.496401s)  | ***1** (0.0553005s)   |
| /                             | 4.35787 (4.21081s)   | ***1** (0.966254s)    |
| /(int)                        | 1.38088 (0.632865s)  | ***1** (0.458306s)    |
| /(unsigned long long)         | 3.60605 (1.64141s)   | ***1** (0.455182s)    |
| /=(unsigned long long)        | 4.03905 (1.73374s)   | ***1** (0.429245s)    |
| construct                     | 3.10884 (0.188684s)  | ***1** (0.0606926s)   |
| construct(unsigned long long) | 9.86198 (1.43813s)   | ***1** (0.145825s)    |
| construct(unsigned)           | 21.2099 (1.39659s)   | ***1** (0.0658462s)   |
| str                           | 1.23162 (0.0016888s) | ***1** (0.0013712s)   |

The platform details for the tests were the following:

```
Platform: Windows x64 
Compiler: Microsoft Visual C++ version 14.2
Boost: 107500
```

## The quad-float implementation
_// TODO_
### `cpp_quad_fp_backend`
_// TODO_

#### Boost.Multiprecision `number<>` wrapper
_// TODO_

#### Constructors
_// TODO_

#### Arithmetic operators
_// TODO_

#### `std::numeric_limits<>` specialization
_// TODO_

#### Tests
_// TODO_

#### Issues
_// TODO List known open and resolved issues_

### Performance
_// TODO_
