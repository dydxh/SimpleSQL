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