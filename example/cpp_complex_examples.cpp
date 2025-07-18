///////////////////////////////////////////////////////////////
//  Copyright 2018 - 2025 Nick Thompson.
//  Copyright 2025 John Maddock.
//  Copyright 2025 Christopher Kormanyos.
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

/*`This example demonstrates the usage of the complex_adaptor backend for multiprecision complex numbers.
In the following, we will show how using the complex_adaptor backend together with number allows for the same operations as the C++ standard library complex numbers.
*/

//[cpp_complex_eg
#include <iostream>
#include <complex>
#include <boost/multiprecision/cpp_complex.hpp>

template<class Complex>
void complex_number_examples()
{
    Complex z1{0, 1};
    std::cout << std::setprecision(std::numeric_limits<typename Complex::value_type>::digits10);
    std::cout << std::scientific << std::fixed;
    std::cout << "Print a complex number   : " << z1 << std::endl;
    std::cout << "Square it                : " << z1*z1 << std::endl;
    std::cout << "Real part                : " << z1.real() << " = " << real(z1) << std::endl;
    std::cout << "Imaginary part           : " << z1.imag() << " = " << imag(z1) << std::endl;
    std::cout << "Absolute value           : " << abs(z1) << std::endl;
    std::cout << "Argument                 : " << arg(z1) << std::endl;
    std::cout << "Norm                     : " << norm(z1) << std::endl;
    std::cout << "Complex conjugate        : " << conj(z1) << std::endl;
    std::cout << "Proj onto Riemann sphere : " <<  proj(z1) << std::endl;
    typename Complex::value_type r = 1;
    typename Complex::value_type theta = 0.8;

    // We need a using declaration here, since polar is called with a scalar:
    using std::polar;

    std::cout << "Polar coord phase =  0   : " << polar(r) << std::endl;
    std::cout << "Polar coord phase != 0   : " << polar(r, theta) << std::endl;

    std::cout << "\nElementary special functions:\n";
    std::cout << "exp(z1)                  : " << exp(z1) << std::endl;
    std::cout << "log(z1)                  : " << log(z1) << std::endl;
    std::cout << "log10(z1)                : " << log10(z1) << std::endl;
    std::cout << "pow(z1, z1)              : " << pow(z1, z1) << std::endl;
    std::cout << "Take its square root     : " << sqrt(z1) << std::endl;
    std::cout << "sin(z1)                  : " << sin(z1) << std::endl;
    std::cout << "cos(z1)                  : " << cos(z1) << std::endl;
    std::cout << "tan(z1)                  : " << tan(z1) << std::endl;
    std::cout << "asin(z1)                 : " << asin(z1) << std::endl;
    std::cout << "acos(z1)                 : " << acos(z1) << std::endl;
    std::cout << "atan(z1)                 : " << atan(z1) << std::endl;
    std::cout << "sinh(z1)                 : " << sinh(z1) << std::endl;
    std::cout << "cosh(z1)                 : " << cosh(z1) << std::endl;
    std::cout << "tanh(z1)                 : " << tanh(z1) << std::endl;
    std::cout << "asinh(z1)                : " << asinh(z1) << std::endl;
    std::cout << "acosh(z1)                : " << acosh(z1) << std::endl;
    std::cout << "atanh(z1)                : " << atanh(z1) << std::endl;
}

int main()
{
    std::cout << "First, some operations performed with std::complex:\n";
    complex_number_examples<std::complex<double>>();

    std::cout << "\nNow the same operations performed with quad precision complex numbers:\n";
    complex_number_examples<boost::multiprecision::cpp_complex_quad>();
}//]

/*

//[cpp_complex_out
Now the same operations performed using quad precision complex numbers:
Print a complex number   : (0.000000000000000000000000000000000,1.000000000000000000000000000000000)
Square it                : -1.000000000000000000000000000000000
Real part                : 0.000000000000000000000000000000000 = 0.000000000000000000000000000000000
Imaginary part           : 1.000000000000000000000000000000000 = 1.000000000000000000000000000000000
Absolute value           : 1.000000000000000000000000000000000
Argument                 : 1.570796326794896619231321691639751
Norm                     : 1.000000000000000000000000000000000
Complex conjugate        : (0.000000000000000000000000000000000,-1.000000000000000000000000000000000)
Proj onto Riemann sphere : (0.000000000000000000000000000000000,1.000000000000000000000000000000000)
Polar coord phase =  0   : 1.000000000000000000000000000000000
Polar coord phase != 0   : (0.696706709347165389063740022772448,0.717356090899522792567167815703377)

Elementary special functions:
exp(z1)                  : (0.540302305868139717400936607442977,0.841470984807896506652502321630299)
log(z1)                  : (0.000000000000000000000000000000000,1.570796326794896619231321691639751)
log10(z1)                : (0.000000000000000000000000000000000,0.682188176920920673742891812715678)
pow(z1, z1)              : 0.207879576350761908546955619834979
Take its square root     : (0.707106781186547524400844362104849,0.707106781186547524400844362104849)
sin(z1)                  : (0.000000000000000000000000000000000,1.175201193643801456882381850595601)
cos(z1)                  : 1.543080634815243778477905620757062
tan(z1)                  : (0.000000000000000000000000000000000,0.761594155955764888119458282604794)
asin(z1)                 : (0.000000000000000000000000000000000,0.881373587019543025232609324979792)
acos(z1)                 : (1.570796326794896619231321691639751,-0.881373587019543025232609324979792)
atan(z1)                 : (0.000000000000000000000000000000000,inf)
sinh(z1)                 : (0.000000000000000000000000000000000,0.841470984807896506652502321630299)
cosh(z1)                 : 0.540302305868139717400936607442977
tanh(z1)                 : (0.000000000000000000000000000000000,1.557407724654902230506974807458360)
asinh(z1)                : (0.000000000000000000000000000000000,1.570796326794896619231321691639751)
acosh(z1)                : (0.881373587019543025232609324979792,1.570796326794896619231321691639751)
atanh(z1)                : (0.000000000000000000000000000000000,0.785398163397448309615660845819876)
//]
*/
