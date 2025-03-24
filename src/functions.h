#include <vector>
#include <string>
#include <cmath>
#include <functional>

// disambiguate overloaded functions 
#define DISAMBIGUATE_MATH_F(x) (double(*)(double))(&x)

namespace kubvc::algorithm::functions
{
    static inline auto ctg(double x) 
    {
        return std::cos(x) / std::sin(x);
    }

    static inline auto arcctg(double x) 
    {
        return std::acos(x / std::sqrt(1 + std::pow(x, 2)));
    }

    // List of generic math functions 
    static const std::vector<std::pair<std::string, std::function<double(double)>>> FunctionList = 
    {        
        { "sin", DISAMBIGUATE_MATH_F(std::sin) },
        { "cos", DISAMBIGUATE_MATH_F(std::cos) },
        { "tg",  DISAMBIGUATE_MATH_F(std::tan) },
        { "ctg", ctg },
     
        { "arccos",  DISAMBIGUATE_MATH_F(std::acos) },
        { "arcsin",  DISAMBIGUATE_MATH_F(std::asin) },
        { "arctg",  DISAMBIGUATE_MATH_F(std::atan) },
        { "arcctg",  DISAMBIGUATE_MATH_F(arcctg) },
        
        { "abs",  DISAMBIGUATE_MATH_F(std::fabs) },
        { "exp",  DISAMBIGUATE_MATH_F(std::exp) },
        { "sqrt",  DISAMBIGUATE_MATH_F(std::sqrt) },
        { "ln",  DISAMBIGUATE_MATH_F(std::log) },
        { "log10",  DISAMBIGUATE_MATH_F(std::log10) },
        { "log2",  DISAMBIGUATE_MATH_F(std::log2) },
    }; 

    static inline std::function<double(double)> getFunction(const std::string& name)
    {
        // Try to find function
        auto result = std::find_if(FunctionList.begin(), FunctionList.end(), [name](std::pair<std::string, std::function<double(double)>> it) {
            return it.first == name;
        });

        // If we are not found function in list 
        if (result == FunctionList.end())
            return nullptr;

        return result->second;
    }
}