#include "Schema.hpp"

bool Schema::checkConstraint(const Limits& limit) {
    for(auto& e : limit) {
        if(e.val.type != attrs[e.attridx]->vtype)
            return false;
    }
    return true;
}