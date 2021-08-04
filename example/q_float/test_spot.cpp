#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "q_float/q_float_qf.h"

namespace local
{
  void test_spot()
  {
    // Set output precision and flags.
    std::cout.precision(std::numeric_limits<q_float>::digits10);
    std::cout.setf(std::ios::showpos | std::ios::uppercase);

    // Test some simple operations on real-valued q_float.
    std::cout << std::endl << "Test some simple operations on real-valued q_float:" << std::endl << std::endl;

    std::cout << q_float(0.0) << std::endl;

    std::cout << qf::pi() << std::endl;

    q_float u_start(123.456);

    q_float u(123.456);

    std::cout << u << std::endl;

    u *= u_start;
    u *= u_start;
    u *= u_start;
    u *= u_start;

    std::cout << u << std::endl;

    u /= u_start;
    u /= u_start;
    u /= u_start;
    u /= u_start;

    std::cout << u << std::endl;

    q_float v(2);

    v.sqrt();

    q_float s(q_float::sqrt2());

    std::cout << s << std::endl;

    const q_float Q(q_float(12345678U) / 1000000UL);

    const q_float E = qf::exp(Q);
    const q_float L = qf::log(E);

    std::cout << "E: " << E << std::endl;
    std::cout << "L: " << L << std::endl;

    const q_float S = qf::sin(qf::one());
    const q_float C = qf::cos(qf::one());

    std::cout << "S: " << S << std::endl;
    std::cout << "C: " << C << std::endl;
    std::cout << qf::asin(qf::half())      << std::endl;
    std::cout << qf::acos(qf::half())      << std::endl;
    std::cout << qf::atan(qf::half())      << std::endl;
    std::cout << qf::cbrt(qf::half())      << std::endl;
    std::cout << qf::rootn(q_float(16), 4) << std::endl;

    // Test some numeric limits of q_float.
    std::cout << std::endl << "Test some numeric limits of q_float:" << std::endl << std::endl;

    std::cout << qf::zero()                                                        << std::endl;
    std::cout << qf::one()                                                         << std::endl;
    std::cout << std::numeric_limits<q_float>::epsilon()                           << std::endl;
    std::cout << qf::one() + std::numeric_limits<q_float>::epsilon()               << std::endl;
    std::cout << qf::one() - std::numeric_limits<q_float>::epsilon()               << std::endl;
    std::cout <<  (std::numeric_limits<q_float>::max)()                            << std::endl;
    std::cout <<  (std::numeric_limits<q_float>::min)()                            << std::endl;
    std::cout <<  (std::numeric_limits<q_float>::min)() * q_float(1E17)            << std::endl;
    std::cout <<  (std::numeric_limits<q_float>::max)() / q_float(1E17)            << std::endl;
    std::cout << ((std::numeric_limits<q_float>::max)() / q_float(3)) * q_float(2) << std::endl;

    // Test some simple operations on complex q_float.
    std::cout << std::endl << "Test some simple operations on complex q_float:" << std::endl << std::endl;

    std::complex<q_float> z(" ( 1.23456 , 7.77888999 )");

    std::cout << z               << std::endl;
    std::cout << qfz::log(z)     << std::endl;
    std::cout << qfz::exp(z)     << std::endl;
    std::cout << qfz::sin(z)     << std::endl;
    std::cout << qfz::cos(z)     << std::endl;
    std::cout << qfz::tan(z)     << std::endl;
    std::cout << z / qfz::exp(z) << std::endl;

    // Test q_float dump operations.
    std::cout << std::endl << "Test q_float dump operations:" << std::endl << std::endl;

    std::string str;

    if(q_float::dump_digits(qfz::tan(z).real(), str))
    {
      std::cout << str << std::endl;
    }

    qfz::tan(z).real().dump();

    std::cout.unsetf(std::ios::dec);
    std::cout.setf(std::ios::hex);

    qfz::tan(z).real().dump();

    std::cout << std::endl << "Test q_float exceptions:" << std::endl << std::endl;

    try
    {
      q_float n(12345678);
      q_float bad = n / qf::zero();
      (void) bad;
    }
    catch(const qf::exception& ex)
    {
      std::cout << ex.what() << std::endl;
    }

    try
    {
      q_float n(1E200);
      q_float bad = n * n;
      (void) bad;
    }
    catch(const qf::exception& ex)
    {
      std::cout << ex.what() << std::endl;
    }

    std::cout.unsetf(std::ios::hex);
    std::cout.setf(std::ios::dec);

    // Test the gamma function which uses q_float.
    std::cout << std::endl << "Test the gamma function:" << std::endl << std::endl;

    std::cout.precision(std::numeric_limits<q_float>::digits10);
    std::cout.setf(std::ios::scientific);

    std::cout << qf::gamma(q_float(10.3)) << std::endl;
    std::cout << qf::gamma(q_float(101))  << std::endl;
    std::cout << qf::gamma(qf::one() / 2) << std::endl;
    std::cout << qf::gamma(qf::one() / 3) << std::endl;
    std::cout << qf::gamma(qf::one() / 4) << std::endl;
    std::cout << qf::gamma(qf::one() / 5) << std::endl;
    std::cout << qf::gamma(qf::one() / 6) << std::endl;
    std::cout << qf::gamma(-qf::half())   << std::endl;

    for(UINT32 i = 0; i < 10; i++)
    {
      std::cout << qfz::gamma(std::complex<q_float>(qf::pi() + i, qf::e() + i)) << std::endl;
    }
  }
}

void test_spot()
{
  local::test_spot();
}
