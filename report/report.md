# Report on Implementation of Double-float and Quad-float classes
## Boost.Multiprecision | Google Summer of Code 2021
---
_Fork link_: [**BoostGSoC2021 _multiprecision_**](https://github.com/BoostGSoC21/multiprecision)

The aim of this project was to set up a practical draft of a generic implementation of the new `Boost.Multiprecision` backends `cpp_double_fp_backend` and `cpp_quad_fp_backend` (which resemble the "double-float" and "quad-float" types that are used to extend the precision of the underlying floating-point types) along with providing compatibility with the Boost.Multiprecision `number<>` wrapper and writing exhaustive tests to prove correctness of the new types.

The mentioned backends implement the underlying arithmetic operators and algorithms on a lower level, while maintaining compatibility with Boost.Multiprecision `number<>` wrapper (through the required [`eval_*()`](https://www.boost.org/doc/libs/1_77_0/libs/multiprecision/test/skeleton_backend.hpp) routines) which allows the end-user and all Boost libraries to interact with the class on a higher level. Finally, the `std::numeric_limits<>` specialization of the classes, along with `number<>` wrapper allows easy integration in the rich library of generic Boost algorithms and tests. Both of the implementations were rigorously tested for correctness.

## The double-float and quad-float implementations
The design philosophy behind the double-float and quad-float implementations was to write a generic template class called `cpp_double_fp_backend`/`cpp_quad_fp_backend` which could theoretically double/quadruple the precision of any IEEE-compaitible floating-point type, and to wrap it up using Boost.Multiprecision `number<>` wrapper. These wrappers are designed to mimic the native floating-point types, through extensive list of constructors and operator overloading:
```
using boost::multiprecision;
cpp_double_float a = 22.0, b = 7, c = "3.1415926535897932384626433832795";
std::cout << "22/7 - pi == " << a / b - c << std::endl;

using boost::multiprecision::pow;
cpp_quad_float pi = "3.1415926535897932384626433832795028841971693993751058209749445923078";
cpp_quad_float e  = "2.7182818284590452353602874713526624977572470936999595749669676277241";
std::cout << pow(e, pi) - pi << std::endl;
```

### Code
The code for implementation of the types can be found in the linked branches:
| Name       | Branch         | Comment |
| ---------- | ------------ | ------- |
| Double-float | [`gsoc2021_double_float`](https://github.com/BoostGSoC21/multiprecision/tree/gsoc2021_double_float) | Double-float implementation and related tests |
| Quad-float | [`gsoc2021_quad_float`](https://github.com/BoostGSoC21/multiprecision/tree/gsoc2021_quad_float/) | Quad-float implementation and related tests |

### Backend types
The backend types constitute of a certain number of non-overlapping native floating-point types, the sum of which equals the represented number. Various algorithms are then used to perform basic arithmetic operations such as addition and multiplication, on top of which more complicated algorithms are built.

#### `cpp_double_fp_backend`
`cpp_double_fp_backend<>` is the backend type behind the implementation of the double-float type. It is a generic template class which constitutes of a pair of two non-overlapping native floating-point types. It has been exhaustively tested with 4-byte `float`, 8-byte `double`, 10-byte `long double`, and 16-byte `float128`.

#### `cpp_quad_fp_backend`
`cpp_quad_fp_backend<>` is the backend type behind the implementation of the quad-float type. Analogous to `cpp_double_fp_backend`, it is a generic template class which constitutes of a 4-tuple of non-overlapping native floating-point types.
It has been exhaustively tested with 8-byte `double`, 10-byte `long double`, and 16-byte `float128`. However, a `cpp_quad_fp_backend` cannot be initialized with a 4-byte `float` because of its extremely limited exponent range.

#### Boost.Multiprecision `number<>` wrapper
The `number<>` wrapper wraps the backend classes to be easily usable by the end-user and compatible with the rest of Boost library, which includes a rich set of mathematical algorithms, generic tests of correctness, and even overloaded constructors and operators for the backend classes. The available `number<>` wrappers for this class are:
```
// Double-float
using cpp_double_float       = number>;
using cpp_double_double      = number>;
using cpp_double_long_double = number>;
using cpp_double_float128    = number>;

// Quad-float
using cpp_quad_double      = number>;
using cpp_quad_long_double = number>;
using cpp_quad_float128    = number>;
```
Note that the availability of `cpp_double_float128` and `cpp_quad_float128` is dependent on availability of `float128`.

#### Constructors
Most of the constructors are straightforward, except for the constructors from arithmetic types which contain more information than what a single underlying native floating-point type can hold. In those cases, special care needs to be taken to extract maximum possible information.

##### Testing
The testing suite for constructors is primarily coded in [`test_cpp_double_float_constructors.cpp`](https://github.com/BoostGSoC21/multiprecision/blob/gsoc2021_double_float/test/test_cpp_double_float_constructors.cpp) for double-float classes and [`test_cpp_quad_float_constructors.cpp`](https://github.com/BoostGSoC21/multiprecision/blob/gsoc2021_quad_float/test/test_cpp_quad_float_constructors.cpp) for quad-float classes. It works by verifying that the representation that is constructed from randomly generated data satisfies the following criteria:
- The maximum error in construction satisfies the bounds of accuracy that are set in the `std::numeric_limits<>` specialization of the class.
- After construction, the underlying floating-points are non-overlapping.

#### Arithmetic operators
The arithmetic operatory algorithms were primarily adaptations of David Bailey's [QD package](https://www.davidhbailey.com/dhbsoftware/qd-2.3.23.tar.gz). The algorithms are described in great detail in [Shewchuk's paper](https://people.eecs.berkeley.edu/~jrs/papers/robustr.pdf), but also in [David Bailey's paper](https://web.mit.edu/tabbott/Public/quaddouble-debian/qd-2.3.4-old/docs/qd.pdf) introducing the QD package.

##### Testing
The arithmetic operatory algorithms were tested for correctness through two series of tests:
- [`test_cpp_double_float_arithmetic.cpp`](https://github.com/BoostGSoC21/multiprecision/blob/gsoc2021_double_float/test/test_cpp_double_float_arithmetic_old.cpp)/[`test_cpp_quad_float_arithmetic.cpp`](https://github.com/BoostGSoC21/multiprecision/blob/gsoc2021_quad_float/test/test_cpp_quad_float_arithmetic.cpp): This homegrown series of tests works by verifying that the error in the arithmetic operations satisfied the bounds set in the `std::numeric_limits<>` specialization of the class.
- `test_arithmetic.cpp`: These were generic, thorough series of tests that are standard to the Boost.Multiprecision library. The scope of this test suite was quite broad: from proper classification of floating-point numbers to correct lexical casting.

#### `std::numeric_limits<>` specialization
Practical and correct values in the `std::numeric_limits<>` specialization of the classes were crucial to the usability of the library. After due discussion, the following table summarizes the consensus on some non-obvious values in the `std::numeric_limits<>` specialization.

|                | `cpp_double_fp_backend<>` | `cpp_quad_fp_backend<>` | Comment |
| -------------- | ------------------------- | ----------------------- | ------- |
| `digits`       | `2 * base_class_digits`   | `4 * base_class_digits` | Reflects the total number of binary digits in the significand |
| `epsilon()`    | `ldexp(1, 4 - digits)`    | `ldexp(1, 6 - digits)`  | Reflects the accuracy of the type |
| `min_exponent` | `base_class_min_exponent + base_class_digits` | `base_class_min_exponent + 3 * base_class_digits` | Reduced to avoid subnormal numbers |
| `max()`        | [link](https://github.com/BoostGSoC21/multiprecision/blob/3a09d7b9b3996e00655f29738b78a4cddf5f971f/include/boost/multiprecision/cpp_double_float.hpp#L924) | [link](https://github.com/BoostGSoC21/multiprecision/blob/78a01080bca5b9cfb9553899c5528172cd6770e3/include/boost/multiprecision/cpp_quad_float.hpp#L800) | Choosen to ensure `sqrt(max())` is computable |
| `min()`        | `ldexp(1, min_exponent)` | `ldexp(1, min_exponent)` | Smallest value representable without using subnormal numbers |

Note that this class has asymmetric exponent range. This is because the value of `min_exponent` is reduced so that none of the constituent floating-point types of the class fall in the subnormal territory. The effects of this asymmetry were much more pronounced in `cpp_quad_fp_backend`, which as a result was [scrapped from implementation](https://github.com/BoostGSoC21/multiprecision/issues/83#issuecomment-898881316).

There was [significant discussion](https://github.com/BoostGSoC21/multiprecision/issues/21) regarding the correct values of `numeric_limits::digits` and `numeric_limits::digits10` for the double-float implementation. The discussion was surrounded around the fact that a few bits of precision are lost during arithmetic operations, and this was needed to be reflected in the `std::numeric_limits<>` specialization for the class. The final [consensus](https://github.com/BoostGSoC21/multiprecision/issues/21#issuecomment-892638810) was that `numeric_limits::epsilon()` will be used to reflect the precision provided by the type instead of `numeric_limits::digits`.

#### Other Tests
The testing workflows also used a substantial subset of standard `Boost.Multiprecision` tests, including tests for arithmetic correctness and tests of elementary transcendental functions. All of these tests, along with a few homegrown tests, were incorporated into a [Jamfile](https://github.com/BoostGSoC21/multiprecision/blob/develop/test/Jamfile.v2) which set into motion Boost-style CI running on GitHubActions flow. The testing workflow also incorporated different datatypes, operating systems, C++ standards, and compilers to ensure maximum compatibility.

The following homegrown tests were also instrumental in proving correctness and catching bugs:
- [`test_cpp_double_float_comparison.cpp`](https://github.com/BoostGSoC21/multiprecision/blob/gsoc2021_double_float/test/test_cpp_double_float_comparision.cpp): Tested the correctness of the comparison operators.
- [`test_cpp_double_float_io.cpp`](https://github.com/BoostGSoC21/multiprecision/blob/gsoc2021_double_float/test/test_cpp_double_float_io.cpp): Tested the string conversion facilities of the class, that are now replaced by the `number<>` wrapper rendering this test defunct.

### Performance
`cpp_double_fp_backend` and `cpp_quad_fp_backend` were tested against `cpp_bin_float` of similar precision. The following table summarizes the relative performance of the types (each operation was performed 10,000 times.)

#### `cpp_double_fp_backend` with `double`
The `cpp_double_fp_backend` class was found to be much faster than than `cpp_bin_float` class of a similar precision.
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


#### `cpp_quad_fp_backend` with `double`
The `cpp_quad_fp_backend` class was found to be only slightly faster than the `cpp_bin_float` class of similar precision, suggesting a lot of room for optimization.
| Operator                      | cpp_bin_float        | cpp_quad_fp_backend |
| ----------------------------- | -------------------- | --------------------- |
| *                      | 1.08476 (0.510203s) |  ***1** (0.47034s) |
| *(int)                 | 1.08811 (0.248002s) |  ***1** (0.227921s) |
| *(unsigned long long)  | ***1** (0.337846s) |  1.53843 (0.519752s) |
| *=(unsigned long long) | ***1** (0.363478s) |  1.34969 (0.490582s) |
| +                      | 1.08739 (0.389422s) |  ***1** (0.358125s) |
| +(int)                 | 4.32414 (0.511417s) |  ***1** (0.11827s) |
| +(unsigned long long)  | 1.7041 (0.478036s) |  ***1** (0.280522s) |
| +=(unsigned long long) | 2.13633 (0.541517s) |  ***1** (0.253481s) |
| -                      | 1.13197 (0.380691s) |  ***1** (0.336309s) |
| -(int)                 | 4.25406 (0.502983s) |  ***1** (0.118236s) |
| -(unsigned long long)  | 1.5853 (0.46435s) |  ***1** (0.292909s) |
| -=(unsigned long long) | 1.78171 (0.454627s) |  ***1** (0.255164s) |
| /                      | 1.2679 (7.21198s) |  ***1** (5.68811s) |
| /(int)                 | ***1** (0.815899s) |  3.06754 (2.5028s) |
| /(unsigned long long)  | 1.0521 (2.61045s) |  ***1** (2.48119s) |
| /=(unsigned long long) | 1.21807 (2.97675s) |  ***1** (2.44382s) |
| construct                     | 1.65911 (0.199257s) |  ***1** (0.120099s) |
| construct(unsigned long long) | 10.8055 (1.47968s) |  ***1** (0.136938s) |
| construct(unsigned)           | 12.1411 (1.47327s) |  ***1** (0.121345s) |
| str                           | ***1** (0.0025268s) |  16.1572 (0.0408261s) |

The platform details for the tests were the following:
```
Platform: Windows x64 
Compiler: Microsoft Visual C++ version 14.2
Boost: 107500
```

## Future work
- Investigate if the precision loss in arithmetic can be eliminated (see [this paper](https://hal.archives-ouvertes.fr/hal-01111551v2))
- Optimize arithmetic algorithms of `cpp_quad_fp_backend` for better performance
- `constexpr`-fication of constructors and arithmetic (see also [issue #37](https://github.com/BoostGSoC21/multiprecision/issues/37))
- Investigate into this strange precision loss issue: [issue #35](https://github.com/BoostGSoC21/multiprecision/issues/35)
