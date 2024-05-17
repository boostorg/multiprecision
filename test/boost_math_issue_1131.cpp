// Copyright 2024 Nick Thompson
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <boost/multiprecision/eigen.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <iostream>

using boost::multiprecision::cpp_bin_float_100;

int main() {
    Eigen::Matrix<cpp_bin_float_100, Eigen::Dynamic, Eigen::Dynamic> A = Eigen::Matrix<cpp_bin_float_100, Eigen::Dynamic, Eigen::Dynamic>::Identity(3,3);
    Eigen::EigenSolver<decltype(A)> es;
    es.compute(A, /*computeEigenvectors=*/ false);

    auto eigs = es.eigenvalues();
    for (std::size_t i = 0; i < eigs.size(); ++i) {
        std::cout << eigs[i] << "\n";
    }

    return 0;
}
