//
// Created by Henry Little on 2019-06-09.
//

#ifndef MINISQL_VALUE_HPP
#define MINISQL_VALUE_HPP

#include <iostream>
#include <cstdio>
#include <vector>
#include <cstring>
#include "../utils/ErrorManager.hpp"

enum class Type {
    INT,
    FLOAT,
    CHAR
};

class MiddleType {
public:
    Type type;
    int clen;
};

extern std::string tostr(Type type);

class Value {
public:
    Type type;
    void *ptr;
    unsigned char clen;

    virtual int size() { return 0; }

    virtual std::string tostr() const {
        switch (type) {
            case Type::INT :
                return std::to_string(*(int *) ptr);
            case Type::FLOAT :
                return std::to_string(*(float *) ptr);
            case Type::CHAR :
                return std::string((char *) ptr);
            default:
                break;
        }
        return "";
    }

    static int valcmp(const Value &lhs, const Value &rhs) {
        if (lhs.type != rhs.type) {
            throw TypeError("[ERROR] Comparing incompatible type");
        } else {
            switch (lhs.type) {
                case Type::INT:
                    return *((int *) lhs.ptr) > *((int *) rhs.ptr) ? 1 : (*((int *) lhs.ptr) < *((int *) rhs.ptr) ? -1
                                                                                                                  : 0);
                case Type::FLOAT:
                    return *((float *) lhs.ptr) > *((float *) rhs.ptr) ? 1 : (*((float *) lhs.ptr) <
                                                                              *((float *) rhs.ptr) ? -1 : 0);
                case Type::CHAR:
                    return strcmp((char *) lhs.ptr, (char *) rhs.ptr);
            }
        }
        return 0;
    }
    
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

#endif //MINISQL_VALUE_HPP
