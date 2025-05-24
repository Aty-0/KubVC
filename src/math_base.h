#pragma once
#include <cmath>
#include <functional>
#include <random>

#include "container.h"
// disambiguate overloaded functions 
#define DISAMBIGUATE_MATH_F(x) (double(*)(double))(&x)

namespace kubvc::math
{
    namespace functions
    {
        static inline auto ctg(double x) 
        {
            return std::cos(x) / std::sin(x);
        }

        static inline auto arcctg(double x) 
        {
            return std::acos(x / std::sqrt(1 + std::pow(x, 2)));
        }

        static inline auto fact(double x)
        {
            return std::tgamma(x + 1);
        }

        static inline auto sh(double x)
        {
            return (std::exp(x) - std::exp(-x)) / 2;
        }

        static inline auto ch(double x)
        {
            return (std::exp(x) + std::exp(-x)) / 2;
        }

        static inline auto th(double x)
        {
            return (std::exp(x) - std::exp(-x)) / (std::exp(x) + std::exp(-x));
        }

        static inline auto cth(double x)
        {
            return 1 / th(x);
        }

        static inline auto sch(double x)
        {
            return 1 / ch(x);
        }

        static inline auto csch(double x)
        {
            return 1 / sh(x);
        }

        static inline auto rnd(double x)
        {
            x = std::fabs(x);
            std::uniform_real_distribution<double> unif(-x, x);
            std::random_device rd;
            std::default_random_engine re(rd());
            auto result = unif(re);
            return result;
        }
    }

    namespace constants
    {
        static constexpr auto PI = 3.14159265358979323846; 
        static constexpr auto E = 2.71828182818284590452; 
    }
    
    namespace containers
    {
        static const kubvc::utility::Container<double> Constants =
        { 
            {
                { "pi", constants::PI },
                { "e", constants::E }
            }
        };

        // List of generic math functions 
        static const kubvc::utility::Container<std::function<double(double)>> Functions =
        {
            {        
                { "sin", DISAMBIGUATE_MATH_F(std::sin) },
                { "cos", DISAMBIGUATE_MATH_F(std::cos) },
                { "tg",  DISAMBIGUATE_MATH_F(std::tan) },
                { "ctg", functions::ctg },

                { "sh", functions::sh  },
                { "ch", functions::ch },
                { "th", functions::th },
                { "cth", functions::cth },
                { "sch", functions::sch },
                { "csch", functions::csch },

                // TODO: Arcs hyperbolic functions

                { "arccos",  DISAMBIGUATE_MATH_F(std::acos) },
                { "arcsin",  DISAMBIGUATE_MATH_F(std::asin) },
                { "arctg",  DISAMBIGUATE_MATH_F(std::atan) },
                { "arcctg",  DISAMBIGUATE_MATH_F(functions::arcctg) },

                { "abs",  DISAMBIGUATE_MATH_F(std::fabs) },
                { "exp",  DISAMBIGUATE_MATH_F(std::exp) },
                { "sqrt",  DISAMBIGUATE_MATH_F(std::sqrt) },
                { "ln",  DISAMBIGUATE_MATH_F(std::log) },
                { "log10",  DISAMBIGUATE_MATH_F(std::log10) },
                { "log2",  DISAMBIGUATE_MATH_F(std::log2) },

                { "fact", functions::fact },
                { "rnd", functions::rnd },
                
                { "round",  DISAMBIGUATE_MATH_F(std::round) },
            }
        }; 
    }
}