#ifndef minisql_attribute_header
#define minisql_attribute_header

#include <cstdio>
#include <iostream>
#include <string>
#include <cstring>
#include <memory>
#include "../Type/Value.hpp"

class Attribute {
public:
    std::string name;
    int unique, clen;
    Type vtype;

    Attribute(const std::string name, const int val) : name(name) {
        unique = val & 1;
        vtype = static_cast<Type>((val >> 1) & 3);
        clen = val >> 3;
    }
    Attribute(const std::string name, const int unique, const int clen, const Type& type) : name(name), unique(unique), clen(clen), vtype(type) {}
    ~Attribute() {};
    
    inline void setattr(int val) {
        unique = val & 1;
        vtype = static_cast<Type>((val >> 1) & 3);
        clen = val >> 3;
    }
    inline int getattr() {
        int retval = 0;
        retval = (clen << 3) | (static_cast<int>(vtype) << 1) | unique;
        return retval;
    }
    inline int size() {
        switch(vtype) {
            case Type::INT :
                return sizeof(int);
            case Type::FLOAT :
                return sizeof(float);
            case Type::CHAR :
                return clen;
        }
        return -1;
    }
};

using AttrPtr = std::shared_ptr<Attribute>;

#endif