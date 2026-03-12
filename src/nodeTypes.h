#pragma once
namespace kubvc::algorithm {
    enum class NodeTypes {
        None,
        Root,
        Number, 
        Variable,
        Function,
        Operator,           
        UnaryOperator,           
        Invalid,           
    };
}