#pragma once
#include <cmath>
#include <numbers>

#include <random>

#include <glm/glm.hpp>

#include "container.h"
#include "window.h"
#include "function_handler.h"

// disambiguate overloaded functions 
#define DISAMBIGUATE_MATH_F(x) (double(*)(double))(&x)

namespace kubvc::math {
    namespace functions {
        static inline constexpr double ctg(double x) {
            return glm::cos(x) / glm::sin(x);
        }

        static inline constexpr double arcctg(double x) {
            return glm::acos(x / glm::sqrt(1 + glm::pow(x, 2)));
        }

        static inline constexpr double fact(double x) {            
            return std::tgamma(x + 1);
        }

        static inline constexpr double sh(double x) {
            return (glm::exp(x) - glm::exp(-x)) / 2;
        }

        static inline constexpr double ch(double x) {
            return (glm::exp(x) + glm::exp(-x)) / 2;
        }

        static inline constexpr double th(double x) {
            return (glm::exp(x) - glm::exp(-x)) / (glm::exp(x) + glm::exp(-x));
        }

        static inline constexpr double cth(double x) {
            return 1 / th(x);
        }

        static inline constexpr double sch(double x) {
            return 1 / ch(x);
        }

        static inline constexpr double csch(double x) {
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
        static constexpr std::initializer_list<std::pair<std::string_view, double>> Constants = { 
                { "invPi", std::numbers::inv_pi_v<double>  },
                { "pi", std::numbers::pi_v<double>  },
                { "e", std::numbers::e_v<double>  },
                { "phi", std::numbers::phi_v<double>  },
                { "egamma",std::numbers::egamma_v<double>  }            
        };
        
        using MathFunctionHandler = utility::FunctionHandler<double(double)>;

        // List of generic math functions 
        static constexpr std::initializer_list<std::pair<std::string_view, MathFunctionHandler>> Functions = {         
                { "sin", glm::sin },
                { "cos", glm::cos },
                { "tg",  glm::tan },
                { "ctg", functions::ctg },

                { "sh", functions::sh  },
                { "ch", functions::ch },
                { "th", functions::th },
                { "cth", functions::cth },
                { "sch", functions::sch },
                { "csch", functions::csch },

                // TODO: Arcs hyperbolic functions

                { "arccos", glm::acos },
                { "arcsin", glm::asin },
                { "arctg",  glm::atan },
                { "arcctg", functions::arcctg },

                { "abs",   glm::abs },
                { "exp",   glm::exp },
                { "sqrt",  glm::sqrt },
                { "ln",    glm::log },
                { "log10", std::log10 },
                { "log2",  glm::log2 },

                { "round", glm::round },
                { "fact", functions::fact },
                
                { "rnd", functions::rnd }      
        }; 
    }
}