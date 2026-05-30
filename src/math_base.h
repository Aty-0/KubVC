#pragma once
#include <cmath>
#include <numbers>
#include <complex>

#include <random>

#include <glm/glm.hpp>

#include "container.h"
#include "window.h"
#include "function_handler.h"

namespace kubvc::math {
    namespace functions {
        namespace complex {
            static inline std::complex<double> abs(const std::complex<double>& z) {
                return { std::abs(z.imag()), std::abs(z.real()) };
            }

            static inline std::complex<double> norm(const std::complex<double>& z) {
                return { std::norm(z), 0.0 };
            }

            static inline std::complex<double> arg(const std::complex<double>& z) {
                return { std::arg(z), 0.0 };
            }

            static inline std::complex<double> real(const std::complex<double>& z) {
                return { 0.0, std::real(z) };
            }

            static inline std::complex<double> imag(const std::complex<double>& z) {
                return { std::imag(z), 0.0 };
            }

            static inline std::complex<double> ctg(const std::complex<double>& z) {
                return std::cos(z) / std::sin(z);
            }

            static inline std::complex<double> arcctg(const std::complex<double>& z) {
                return std::atan(1.0 / z);
            }
            
            static inline std::complex<double> cth(const std::complex<double>& z) {
                return std::cosh(z) / std::sinh(z);
            }

            static inline std::complex<double> sch(const std::complex<double>& z) {
                return 1.0 / std::cosh(z);
            }
        
            static inline std::complex<double> csch(const std::complex<double>& z) {
                return 1.0 / std::sinh(z);
            }
            
            static inline std::complex<double> arcth(const std::complex<double>& z) {
                return std::atanh(1.0 / z);
            }
            
            static inline std::complex<double> log2(const std::complex<double>& z) {
                return  std::log(z) / std::log(2.0);                
            }
        }
        
        namespace real {        
            static inline double ctg(double x) {
                return glm::cos(x) / glm::sin(x);
            }

            static inline double arcctg(double x) {
                return glm::acos(x / glm::sqrt(1 + glm::pow(x, 2)));
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
    }
    
    namespace containers {
        static constexpr std::initializer_list<std::pair<std::string_view, double>> Constants = { 
                { "invPi", std::numbers::inv_pi_v<double>  },
                { "pi", std::numbers::pi_v<double>  },
                { "e", std::numbers::e_v<double>  },
                { "phi", std::numbers::phi_v<double>  },
                { "egamma",std::numbers::egamma_v<double>  }            
        };
        
        using RealFunctionHandler = utility::FunctionHandler<double(double)>;

        // List of generic math functions 
        static constexpr std::initializer_list<std::pair<std::string_view, RealFunctionHandler>> Functions = {         
                { "sin", glm::sin },
                { "cos", glm::cos },
                { "tg",  glm::tan },
                { "ctg", functions::real::ctg },

                { "sh", functions::real::sh  },
                { "ch", functions::real::ch },
                { "th", functions::real::th },
                { "cth", functions::real::cth },
                { "sch", functions::real::sch },
                { "csch", functions::real::csch },

                // TODO: Arcs hyperbolic functions

                { "arccos", glm::acos },
                { "arcsin", glm::asin },
                { "arctg",  glm::atan },
                { "arcctg", functions::real::arcctg },

                { "abs",   glm::abs },
                { "exp",   glm::exp },
                { "sqrt",  glm::sqrt },
                { "norm",  std::norm },
                { "arg",   std::arg },

                { "ln",    glm::log },
                { "log10", std::log10 },
                { "log2",  glm::log2 },

                { "round", glm::round },
                { "fact", functions::real::fact },
                
                { "rnd", functions::real::rnd }      
        }; 

        using ComplexFunctionHandler = utility::FunctionHandler<std::complex<double>(const std::complex<double>&)>;

        static constexpr std::initializer_list<std::pair<std::string_view, ComplexFunctionHandler>> ComplexFunctions = {         
                { "sin",   std::sin },
                { "cos", std::cos },
                { "tg", std::tan },
                { "ctg",   functions::complex::ctg },
                
                { "arcsin", std::asin },
                { "arccos", std::acos },
                { "arctg", std::atan },
                { "arcctg", functions::complex::arcctg },
                
                { "sh", std::sinh },
                { "ch", std::cosh },
                { "th", std::tanh },
                { "cth",   functions::complex::cth },
                { "sch",   functions::complex::sch },
                { "csch",  functions::complex::csch },
                
                { "arsh",  std::asinh },
                { "arch",  std::acosh },
                { "arth",  std::atanh },
                { "arcth", functions::complex::arcth },
                
                { "exp",   std::exp },
                { "ln",    std::log },
                { "log10", std::log10 },
                { "log2",  functions::complex::log2 },
                
                { "sqrt",  std::sqrt },
                { "abs",   functions::complex::abs },
                { "norm",  functions::complex::norm },
                { "arg",   functions::complex::arg },
                { "conj",  std::conj },
                { "proj",  std::proj },
                
                { "re", functions::complex::real },
                { "im", functions::complex::imag }              
        };
    }
}