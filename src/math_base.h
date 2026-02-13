#pragma once
#include <cmath>
#include <functional>
#include <numbers>

#include "container.h"

#include <random>
#include "window.h"

#include <glm/glm.hpp>

// disambiguate overloaded functions 
#define DISAMBIGUATE_MATH_F(x) (double(*)(double))(&x)

namespace kubvc::math {
    namespace functions {
        static inline double ctg(double x) {
            return glm::cos(x) / std::sin(x);
        }

        static inline double arcctg(double x) {
            return glm::acos(x / std::sqrt(1 + std::pow(x, 2)));
        }

        static inline double fact(double x) {            
            return std::tgamma(x + 1);
        }

        static inline double sh(double x) {
            return (glm::exp(x) - glm::exp(-x)) / 2;
        }

        static inline double ch(double x) {
            return (glm::exp(x) + glm::exp(-x)) / 2;
        }

        static inline double th(double x) {
            return (glm::exp(x) - glm::exp(-x)) / (glm::exp(x) + glm::exp(-x));
        }

        static inline double cth(double x) {
            return 1 / th(x);
        }

        static inline double sch(double x) {
            return 1 / ch(x);
        }

        static inline double csch(double x) {
            return 1 / sh(x);
        }

        static inline double rnd(double x) {
            x = glm::abs(x);
            std::uniform_real_distribution<double> unif(-x, x);
            std::random_device rd;
            std::default_random_engine re(rd());
            auto result = unif(re);
            return result;
        }
    }
    
    namespace containers {
        static const kubvc::utility::Container<double> Constants = { {
                { "invPi", std::numbers::inv_pi_v<double>  },
                { "pi", std::numbers::pi_v<double>  },
                { "e", std::numbers::e_v<double>  },
                { "phi", std::numbers::phi_v<double>  },
                { "egamma",std::numbers::egamma_v<double>  }
            }
        };

        // List of generic math functions 
        static const kubvc::utility::Container<std::function<double(double)>> Functions = { {        
                { "sin", DISAMBIGUATE_MATH_F(glm::sin) },
                { "cos", DISAMBIGUATE_MATH_F(glm::cos) },
                { "tg",  DISAMBIGUATE_MATH_F(glm::tan) },
                { "ctg", functions::ctg },

                { "sh", functions::sh  },
                { "ch", functions::ch },
                { "th", functions::th },
                { "cth", functions::cth },
                { "sch", functions::sch },
                { "csch", functions::csch },

                // TODO: Arcs hyperbolic functions

                { "arccos", DISAMBIGUATE_MATH_F(glm::acos) },
                { "arcsin", DISAMBIGUATE_MATH_F(glm::asin) },
                { "arctg",  DISAMBIGUATE_MATH_F(glm::atan) },
                { "arcctg", functions::arcctg },

                { "abs",   DISAMBIGUATE_MATH_F(glm::abs) },
                { "exp",   DISAMBIGUATE_MATH_F(glm::exp) },
                { "sqrt",  DISAMBIGUATE_MATH_F(glm::sqrt) },
                { "ln",    DISAMBIGUATE_MATH_F(glm::log) },
                { "log10", DISAMBIGUATE_MATH_F(std::log10) },
                { "log2",  DISAMBIGUATE_MATH_F(glm::log2) },

                { "round", DISAMBIGUATE_MATH_F(glm::round) },
                { "fact", functions::fact },
                
                { "rnd", functions::rnd }      
            }
        }; 
    }
}