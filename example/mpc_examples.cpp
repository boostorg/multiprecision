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
    //std::cout << std::setprecision(std::numeric_limits<typename Complex::value_type>::digits10);
    std::cout << std::scientific << std::fixed;
    std::cout << "Print a complex number: " << z1 << std::endl;
    std::cout << "Square it             : " << z1*z1 << std::endl;
    using std::sqrt;
    std::cout << "Take its square root  : " << sqrt(z1) << std::endl;
    std::cout << "Real part             : " << z1.real() << " = " << real(z1) << std::endl;
    std::cout << "Imaginary part        : " << z1.imag() << " = " << imag(z1) << std::endl;
    using std::abs;
    std::cout << "Absolute value        : " << abs(z1) << std::endl;
    std::cout << "Argument              : " << arg(z1) << std::endl;
    std::cout << "Norm                  : " << norm(z1) << std::endl;
    std::cout << "Complex conjugate     : " << conj(z1) << std::endl;
    std::cout << "Projection onto Riemann sphere: " <<  proj(z1) << std::endl;
    std::cout << "Polar coordinates     : " << polar(z1) << std::endl;

    std::cout << "\nElementary special functions:\n";
    using std::exp;
    std::cout << "exp(z1) = " << exp(z1) << std::endl;
    using std::log;
    std::cout << "log(z1) = " << log(z1) << std::endl;
    using std::log10;
    std::cout << "log10(z1) = " << log10(z1) << std::endl;
    using std::pow;
    std::cout << "pow(z1, z1) = " << pow(z1, z1) << std::endl;

    // This doesn't work form MPC!
    //std::cout << "Reinterpret cast to real part: " << reinterpret_cast<Complex(&)[2]>(z1)[0] << std::endl;
    //std::cout << "Reinterpret cast to imaginary part: " << reinterpret_cast<Complex(&)[2]>(z1)[1] << std::endl;

}

int main()
{
    std::cout << "First, some operations we usually perform with std::complex:\n";
    complex_number_examples<std::complex<double>>();
    std::cout << "Now the same operations performed using the MPC backend:\n";
    complex_number_examples<boost::multiprecision::mpc_complex_100>();

    return 0;
}
