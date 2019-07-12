#ifndef CRYPTO3_REDUCE_BELOW_HPP
#define CRYPTO3_REDUCE_BELOW_HPP

namespace nil {
    namespace crypto3 {
        using namespace boost::multiprecision;

        template<typename Backend>
        void eval_reduce_below(Backend &val, const Backend &mod) {
            using default_ops::eval_lt;

            if (eval_lt(mod, 0)) {
                BOOST_THROW_EXCEPTION(std::invalid_argument("reduce_below modulus must be positive"));
            }

            while (eval_lt(mod, val))  {
                eval_subtract(val, mod);
            }
        }

        template<typename Backend, expression_template_option ExpressionTemplates>
        number <Backend, ExpressionTemplates> reduce_below(number <Backend, ExpressionTemplates> &val,
                                                           const number <Backend, ExpressionTemplates> &mod) {
            return number<Backend, ExpressionTemplates>(eval_reduce_below(val.backend(), mod.backend()));
        }
    }
}

#endif //CRYPTO3_REDUCE_BELOW_HPP
