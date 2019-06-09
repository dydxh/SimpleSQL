//
// Created by Henry Little on 2019-06-09.
//

#ifndef MINISQL_VALUE_HPP
#define MINISQL_VALUE_HPP

#include <iostream>
#include <cstdio>
#include <cstring>
#include "../utils/ErrorManager.hpp"

enum class Type {
    INT,
    FLOAT,
    CHAR
};

class Value {
public:
    Type type;
    void *ptr;
    unsigned char charlen;
};

class IntValue : public Value {
    IntValue(int *ptr_) {
        this->type = Type::INT;
        this->ptr = ptr_;
    }
};

class FloatValue : public Value {
    FloatValue(float *ptr_) {
        this->type = Type::FLOAT;
        this->ptr = ptr_;
    }
};

class CharValue : public Value {
    CharValue(char *ptr_, unsigned char len_) {
        this->type = Type::CHAR;
        this->ptr = ptr_;
        this->charlen = len_;
    }
};

static int valcmp(Value a, Value b) {
    if (a.type != b.type) {
        throw TypeError("[ERROR] Comparing incompatible type");
    } else {
        switch (a.type) {
            case Type::INT:
                return *((int *) a.ptr) > *((int *) b.ptr) ? 1 : (*((int *) a.ptr) < *((int *) b.ptr) ? -1 : 0);
            case Type::FLOAT:
                return *((float *) a.ptr) > *((float *) b.ptr) ? 1 : (*((float *) a.ptr) < *((float *) b.ptr) ? -1 : 0);
            case Type::CHAR:
                return strcmp((char *)a.ptr, (char *)b.ptr);
        }
    }
}

#endif //MINISQL_VALUE_HPP
