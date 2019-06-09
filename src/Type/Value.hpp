//
// Created by Henry Little on 2019-06-09.
//

#ifndef MINISQL_VALUE_HPP
#define MINISQL_VALUE_HPP

class Value {
public:
    Type type;
    void *ptr;
    unsigned char charlen;
    virtual int size(){};
};

class IntValue : public Value {
    Value::type = Type::INT;

    IntValue(int *ptr_) : Value::ptr(ptr_) {}
};

class FloatValue : public Value {
    Value::type = Type::FLOAT;

    FloatValue(float *ptr_) : Value::ptr(ptr_) {}
};

class CharValue : public Value {
    Value::type = Type::CHAR;

    CharValue(char *ptr_) : Value::ptr(ptr_) {}
};

enum class Type {
    INT,
    FLOAT,
    CHAR
};

static int valcmp(Value a, Value b) {
    if (a.type != b.type) {
        std::cout << "[ERROR] Comparing incompatible type" << std::endl;
    } else {
        switch (a.type) {
            case Type::INT:
                return *((int *) a.ptr) > *((int *) b.ptr) ? 1 : (*((int *) a.ptr) < *((int *) b.ptr) ? -1 : 0);
            case Type::FLOAT:
                return *((float *) a.ptr) > *((float *) b.ptr) ? 1 : (*((float *) a.ptr) < *((float *) b.ptr) ? -1 : 0);
            case Type::CHAR:
                return strncmp((char *)a.ptr, (char *)b.ptr, a.);
        }
    }
}

#endif //MINISQL_VALUE_HPP
