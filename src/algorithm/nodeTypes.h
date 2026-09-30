#pragma once
namespace kubvc::algorithm {
    enum class NodeTypes {
        None,
        Root,
        Number, 
        ComplexNumber,      
        Variable,
        Function,
        Operator,           
        UnaryOperator,     
        Invalid,           
    };
}