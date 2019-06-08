#ifndef minisql_attribute_header
#define minisql_attribute_header

#include <cstdio>
#include <iostream>
#include <string>
#include <cstring>
#include <memory>

enum class Valuetype {
    INT,
    FLOAT,
    CHAR
};

class Attribute {
public:
    std::string name;
    int unique, clen;
    Valuetype vtype;

    Attribute(const std::string name, const int val) : name(name) {
        unique = val & 1;
        vtype = static_cast<Valuetype>((val >> 1) & 3);
        clen = val >> 3;
    }
    ~Attribute() {};
    
    inline void setattr(int val) {
        unique = val & 1;
        vtype = static_cast<Valuetype>((val >> 1) & 3);
        clen = val >> 3;
    }
    inline int getattr() {
        int retval = 0;
        retval = (clen << 3) | (static_cast<int>(vtype) << 1) | unique;
        return retval;
    }
};

using AttrPtr = std::shared_ptr<Attribute>;

#endif