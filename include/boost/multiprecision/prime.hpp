#ifndef CRYPTO3_NUMBER_THEORY_H_
#define CRYPTO3_NUMBER_THEORY_H_

#include <boost/hash/sha.hpp>

#include <boost/multiprecision/pow_mod.hpp>
#include <boost/multiprecision/modular_reduce.hpp>

#include <boost/multiprecision/mp_core???.hpp>

#include <boost/utilities/ct_utils.hpp>
#include <boost/utilities/bit_ops.hpp>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/miller_rabin.hpp>

namespace nil {
    namespace crypto3 {

        using namespace boost::multiprecision;

        namespace detail {
            /*
             * Check if this size is allowed by FIPS 186-3
             */
            bool fips186_3_valid_size(size_t pbits, size_t qbits) {
                if (qbits == 160) {
                    return (pbits == 1024);
                }

                if (qbits == 224) {
                    return (pbits == 2048);
                }

                if (qbits == 256) {
                    return (pbits == 2048 || pbits == 3072);
                }

                return false;
            }

            /**
             * The size of the PRIMES[] array
             */
            const size_t PRIME_TABLE_SIZE = 6541;

            /**
             * A const array of all primes less than 65535
             */
            extern const uint16_t PRIMES[];
        }

        /**
         * Compute -input^-1 mod 2^MP_WORD_BITS. Returns zero if input
         * is even. If input is odd, input and 2^n are relatively prime
         * and an inverse exists.
         */
        template<typename Word>
        typename std::enable_if<number_category<Word>::value == number_kind_integer, Word>::type monty_inverse(
                Word input) {
            if (input == 0) {
                BOOST_THROW_EXCEPTION(std::invalid_argument("monty_inverse: divide by zero"));
            }

            Word b = input;
            Word x2 = 1, x1 = 0, y2 = 0, y1 = 1;

            // First iteration, a = n+1
            Word q = bigint_divop(1, 0, b);
            Word r = (MP_WORD_MAX - q * b) + 1;
            Word x = x2 - q * x1;
            Word y = y2 - q * y1;

            Word a = b;
            b = r;
            x2 = x1;
            x1 = x;
            y2 = y1;
            y1 = y;

            while (b > 0) {
                q = a / b;
                r = a - q * b;
                x = x2 - q * x1;
                y = y2 - q * y1;

                a = b;
                b = r;
                x2 = x1;
                x1 = x;
                y2 = y1;
                y1 = y;
            }

            const Word check = y2 * input;
            CRYPTO3_ASSERT_EQUAL(check, 1, "monty_inverse result is inverse of input");

            // Now invert in addition space
            y2 = (MP_WORD_MAX - y2) + 1;

            return y2;
        }

        /**
         * Randomly generate a prime
         * @param rng a random number generator
         * @param bits how large the resulting prime should be in bits
         * @param coprime a positive integer that (prime - 1) should be coprime to
         * @param equiv a non-negative number that the result should be
                        equivalent to modulo equiv_mod
         * @param equiv_mod the modulus equiv should be checked against
         * @param prob use test so false positive is bounded by 1/2**prob
         * @return random prime with the specified criteria
         */
        template<typename Backend, expression_template_option ExpressionTemplates,
                 typename UniformRandomNumberGenerator>
        number<Backend, ExpressionTemplates> random_prime(UniformRandomNumberGenerator &rng, size_t bits,
                                                          const number<Backend, ExpressionTemplates> &coprime = 0,
                                                          std::size_t equiv = 1, std::size_t modulo = 2,
                                                          std::size_t prob = 128) {
            if (coprime < 0) {
                BOOST_THROW_EXCEPTION(std::invalid_argument("random_prime: coprime must be >= 0"));
            }
            if (modulo == 0) {
                BOOST_THROW_EXCEPTION(std::invalid_argument("random_prime: Invalid modulo value"));
            }

            equiv %= modulo;

            if (equiv == 0) {
                BOOST_THROW_EXCEPTION(std::invalid_argument("random_prime Invalid value for equiv/modulo"));
            }

            // Handle small values:
            if (bits <= 1) {
                BOOST_THROW_EXCEPTION(
                        std::invalid_argument("random_prime: Can't make a prime of " + std::to_string(bits) + " bits"));
            } else if (bits == 2) {
                return ((rng.next_byte() % 2) ? 2 : 3);
            } else if (bits == 3) {
                return ((rng.next_byte() % 2) ? 5 : 7);
            } else if (bits == 4) {
                return ((rng.next_byte() % 2) ? 11 : 13);
            } else if (bits <= 16) {
                for (;;) {
                    size_t idx = make_uint16(rng.next_byte(), rng.next_byte()) % detail::PRIME_TABLE_SIZE;
                    uint16_t small_prime = detail::PRIMES[idx];

                    if (high_bit(small_prime) == bits) {
                        return small_prime;
                    }
                }
            }

            secure_vector<uint16_t> sieve(detail::PRIME_TABLE_SIZE);
            const size_t MAX_ATTEMPTS = 32 * 1024;

            while (true) {
                cpp_int p(rng, bits);

                // Force lowest and two top bits on
                bit_set(p, bits - 1);
                bit_set(p, bits - 2);
                bit_set(p, 0);

                // Force p to be equal to equiv mod modulo
                p += (modulo - (p % modulo)) + equiv;

                for (size_t i = 0; i != sieve.size(); ++i) {
                    sieve[i] = static_cast<uint16_t>(p % detail::PRIMES[i]);
                }

                size_t counter = 0;
                while (true) {
                    ++counter;

                    if (counter > MAX_ATTEMPTS) {
                        break; // don't try forever, choose a new starting point
                    }

                    p += modulo;

                    if (msb(p) > bits) {
                        break;
                    }

                    // Now that p is updated, update the sieve
                    for (size_t i = 0; i != sieve.size(); ++i) {
                        sieve[i] = (sieve[i] + modulo) % detail::PRIMES[i];
                    }

                    bool passes_sieve = true;
                    for (size_t i = 0; passes_sieve && (i != sieve.size()); ++i) {
                        /*
                        In this case, p is a multiple of PRIMES[i]
                        */
                        if (sieve[i] == 0) {
                            passes_sieve = false;
                        }

                        /*
                        In this case, 2*p+1 will be a multiple of PRIMES[i]

                        So if generating a safe prime, we want to avoid this value
                        because 2*p+1 will not be useful. Since the check is cheap to
                        do and doesn't seem to affect the overall distribution of the
                        generated primes overmuch it's used in all cases.

                        See "Safe Prime Generation with a Combined Sieve" M. Wiener
                        https://eprint.iacr.org/2003/186.pdf
                        */
                        if (sieve[i] == (detail::PRIMES[i] - 1) / 2) {
                            passes_sieve = false;
                        }
                    }

                    if (!passes_sieve) {
                        continue;
                    }

                    if (coprime > 0 && gcd(p - 1, coprime) != 1) {
                        continue;
                    }

                    if (miller_rabin_test(p, prob, rng)) {
                        return p;
                    }
                }
            }
        }

        /**
         * Return a 'safe' prime, of the form p=2*q+1 with q prime
         * @param rng a random number generator
         * @param bits is how long the resulting prime should be
         * @return prime randomly chosen from safe primes of length bits
         */
        template<typename UniformRandomNumberGenerator,
                 typename Backend, expression_template_option ExpressionTemplates>
        number<Backend, ExpressionTemplates> random_safe_prime(UniformRandomNumberGenerator &rng, std::size_t bits) {
            if (bits <= 64) {
                BOOST_THROW_EXCEPTION(std::invalid_argument(
                        "random_safe_prime: Can't make a prime of " + std::to_string(bits) + " bits"));
            }

            cpp_int q, p;
            for (;;) {
                /*
                Generate q == 2 (mod 3)

                Otherwise [q == 1 (mod 3) case], 2*q+1 == 3 (mod 3) and not prime.
                */
                q = random_prime(rng, bits - 1, 1, 2, 3, 8);
                p = (q << 1) + 1;

                if (miller_rabin_test(p, 128, rng)) {
                    // We did only a weak check before, go back and verify q before returning
                    if (miller_rabin_test(q, 128, rng)) {
                        return p;
                    }
                }

            }
        }

        /**
         * @brief Generate DSA parameters using the FIPS 186 kosherizer
         *
         * @tparam Hasher
         * @tparam UniformRandomNumberGenerator
         *
         * @param rng a random number generator
         * @param p_out where the prime p will be stored
         * @param q_out where the prime q will be stored
         * @param pbits how long p will be in bits
         * @param qbits how long q will be in bits
         * @param seed_c the seed used to generate the parameters
         * @param offset optional offset from seed to start searching at
         * @return true if seed generated a valid DSA parameter set, otherwise
                  false. p_out and q_out are only valid if true was returned.
         */
        template<template<std::size_t QBits> class hash::sha<QBits>, std::size_t PBits, std::size_t QBits,
                typename Backend, expression_template_option ExpressionTemplates, typename UniformRandomNumberGenerator>
        bool generate_dsa_primes(number<Backend, ExpressionTemplates> &q_out, const std::vector<uint8_t> &seed_c,
                                 size_t offset = 0, UniformRandomNumberGenerator &rng, Hasher &hasher) {
            if (!detail::fips186_3_valid_size(PBits, QBits)) {
                BOOST_THROW_EXCEPTION(std::invalid_argument(
                        "FIPS 186-3 does not allow DSA domain parameters of " + std::to_string(PBits) + "/" +
                        std::to_string(QBits) + " bits long"));
            }

            if (seed_c.size() * 8 < QBits) {
                BOOST_THROW_EXCEPTION(std::invalid_argument(
                        "Generating a DSA parameter set with a " + std::to_string(QBits) +
                        " bit long q requires a seed at least as many bits long"));
            }

            const std::string hash_name = "SHA-" + std::to_string(QBits);
            std::unique_ptr<HashFunction> hash(HashFunction::create_or_throw(hash_name));

            const size_t HASH_SIZE = hash->output_length();

            class Seed final {
            public:
                explicit Seed(const std::vector<uint8_t> &s) : m_seed(s) {
                }

                const std::vector<uint8_t> &value() const {
                    return m_seed;
                }

                Seed &operator++() {
                    for (size_t j = m_seed.size(); j > 0; --j) {
                        if (++m_seed[j - 1]) {
                            break;
                        }
                    }
                    return (*this);
                }

            private:
                std::vector<uint8_t> m_seed;
            };

            Seed seed(seed_c);

            q_out.binary_decode(hash->process(seed.value()));
            bit_set(q_out, QBits - 1);
            bit_set(q_out, 0);

            if (!miller_rabin_test(q_out, 126, rng)) {
                return false;
            }

            const size_t n = (PBits - 1) / (HASH_SIZE * 8), b = (PBits - 1) % (HASH_SIZE * 8);

            cpp_int X;
            std::vector<uint8_t> V(HASH_SIZE * (n + 1));

            for (size_t j = 0; j != 4 * PBits; ++j) {
                for (size_t k = 0; k <= n; ++k) {
                    ++seed;
                    hash->update(seed.value());
                    hash->final(&V[HASH_SIZE * (n - k)]);
                }

                if (j >= offset) {
                    X.binary_decode(&V[HASH_SIZE - 1 - b / 8], V.size() - (HASH_SIZE - 1 - b / 8));
                    bit_set(X, PBits - 1);

                    p_out = X - (X % (2 * q_out) - 1);

                    if (p_out.bits() == PBits && miller_rabin_test(p_out, 126, rng)) {
                        return true;
                    }
                }
            }
            return false;
        }
    }
}

#endif