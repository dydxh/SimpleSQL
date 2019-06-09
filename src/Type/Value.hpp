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
    unsigned char clen;

    virtual int size() {return 0;}
};

class IntValue : public Value {
    IntValue(int *ptr_) {
        this->type = Type::INT;
        this->ptr = ptr_;
    }
    int size() override {
        return sizeof(int);
    }
};

class FloatValue : public Value {
    FloatValue(float *ptr_) {
        this->type = Type::FLOAT;
        this->ptr = ptr_;
    }
    int size() override {
        return sizeof(float);
    }
};

class CharValue : public Value {
    CharValue(char *ptr_, unsigned char clen_) {
        this->type = Type::CHAR;
        this->ptr = ptr_;
        this->clen = clen_;
    }
    int size() override {
        return clen;
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
