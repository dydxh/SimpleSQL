#ifndef minisql_constraint_header
#define minisql_constraint_header

#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include "Value.hpp"

enum class Operator {
    EQ, NEQ, GT, LT, LEQ, GEQ
};

class RawConstraint {
public:
    std::string name;
    Operator op;
    Value val;
};

class Constraint {
public:
    int attridx;
    Operator op;
    Value val;

    static bool valcmp(const Operator& op, const Value& lhs, const Value& rhs) {
        int res = Value::valcmp(lhs, rhs);
        switch (op) {
        case Operator::EQ :
            return res == 0;
        case Operator::NEQ :
            return res != 0;
        case Operator::GT :
            return res == 1;
        case Operator::LT :
            return res == -1;
        case Operator::GEQ :
            return res != -1;
        case Operator::LEQ :
            return res != 1;
        default:
            return false;
        }
        return false;
    }
};



using RawLimits = std::vector<RawConstraint>;
using Limits = std::vector<Constraint>;

#endif