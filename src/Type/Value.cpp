#include "Value.hpp"

std::string tostr(Type type) {
    switch (type) {
    case Type::INT :
        return "Int";
        break;
    case Type::FLOAT :
        return "Float";
        break;
    case Type::CHAR :
        return "Chat*";
        break;
    default:
        return "[Unkown]";
        break;
    }
}
