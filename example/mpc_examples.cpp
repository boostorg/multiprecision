///////////////////////////////////////////////////////////////
//  Copyright 2018 Nick Thompson. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

/*`This example demonstrates the usage of the MPC backend for multiprecision complex numbers.
In the following, we will show how using MPC backend allows for the same operations as the C++ standard library complex numbers.
*/

#include <iostream>
#include <complex>
#include <boost/multiprecision/mpc.hpp>

template<class Complex>
void complex_number_examples()
{
    Complex z1{0, 1};
    std::cout << std::setprecision(std::numeric_limits<typename Complex::value_type>::digits10);
    std::cout << std::scientific << std::fixed;
    std::cout << "Print a complex number: " << z1 << std::endl;
    std::cout << "Square it             : " << z1*z1 << std::endl;
    std::cout << "Real part             : " << z1.real() << " = " << real(z1) << std::endl;
    std::cout << "Imaginary part        : " << z1.imag() << " = " << imag(z1) << std::endl;
    using std::abs;
    std::cout << "Absolute value        : " << abs(z1) << std::endl;
    std::cout << "Argument              : " << arg(z1) << std::endl;
    std::cout << "Norm                  : " << norm(z1) << std::endl;
    std::cout << "Complex conjugate     : " << conj(z1) << std::endl;
    std::cout << "Projection onto Riemann sphere: " <<  proj(z1) << std::endl;
    typename Complex::value_type r = 1;
    typename Complex::value_type theta = 0.8;
    using std::polar;
    std::cout << "Polar coordinates (phase = 0)    : " << polar(r) << std::endl;
    std::cout << "Polar coordinates (phase !=0)    : " << polar(r, theta) << std::endl;

    std::cout << "\nElementary special functions:\n";
    using std::exp;
    std::cout << "exp(z1) = " << exp(z1) << std::endl;
    using std::log;
    std::cout << "log(z1) = " << log(z1) << std::endl;
    using std::log10;
    std::cout << "log10(z1) = " << log10(z1) << std::endl;
    using std::pow;
    std::cout << "pow(z1, z1) = " << pow(z1, z1) << std::endl;
    using std::sqrt;
    std::cout << "Take its square root  : " << sqrt(z1) << std::endl;
    using std::sin;
    std::cout << "sin(z1) = " << sin(z1) << std::endl;
    using std::cos;
    std::cout << "cos(z1) = " << cos(z1) << std::endl;
    using std::tan;
    std::cout << "tan(z1) = " << tan(z1) << std::endl;
    using std::asin;
    std::cout << "asin(z1) = " << asin(z1) << std::endl;
    using std::acos;
    std::cout << "acos(z1) = " << acos(z1) << std::endl;
    using std::atan;
    std::cout << "atan(z1) = " << atan(z1) << std::endl;
    using std::sinh;
    std::cout << "sinh(z1) = " << sinh(z1) << std::endl;
    using std::cosh;
    std::cout << "cosh(z1) = " << cosh(z1) << std::endl;
    using std::tanh;
    std::cout << "tanh(z1) = " << tanh(z1) << std::endl;
    using std::asinh;
    std::cout << "asinh(z1) = " << asinh(z1) << std::endl;
    using std::acosh;
    std::cout << "acosh(z1) = " << acosh(z1) << std::endl;
    using std::atanh;
    std::cout << "atanh(z1) = " << atanh(z1) << std::endl;

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wstrict-aliasing"
    std::cout << "Reinterpret cast to real part: " << reinterpret_cast<typename Complex::value_type(&)[2]>(z1)[0] << std::endl;
    std::cout << "Reinterpret cast to imaginary part: " << reinterpret_cast<typename Complex::value_type(&)[2]>(z1)[1] << std::endl;
    #pragma GCC diagnostic pop

    using namespace std::complex_literals;
    Complex z3 = 1. + 2i;
}

int main()
{
    std::cout << "First, some operations we usually perform with std::complex:\n";
    complex_number_examples<std::complex<double>>();
    std::cout << "\nNow the same operations performed using the MPC backend:\n";
    complex_number_examples<boost::multiprecision::mpc_complex_50>();

    return 0;
}
