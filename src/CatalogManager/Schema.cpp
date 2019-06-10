#include "Schema.hpp"

bool Schema::checkConstraint(const Limits& limit) {
    for(auto& e : limit) {
        if(e.val.type != attrs[e.attridx]->vtype)
            return false;
    }
    return true;
}

int Schema::getidx(const std::string& name) {
    for(int i = 0; i < attrs.size(); i++)
        if(attrs[i]->name == name)
            return i;
    return -1;
}

Limits Schema::translimits(const RawLimits& rawlimits) {
    Limits limit;
    for(auto& e : rawlimits) {
        Constraint val;
        val.attridx = getidx(e.name);
        if(val.attridx == -1) 
            throw AttributeError("[Error] Get unkown attribute '" + e.name + "'.");
        val.op = e.op;
        val.val = e.val;
        limit.push_back(val);
    }
    return limit;
}