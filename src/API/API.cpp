#include "API.hpp"
#include "../Interpreter/Driver.hpp"

// API::API(const CatalogPtr& catalog) {
//     this->catalog = catalog;
// }
// API::~API() {

// }

CatalogPtr API::catalog;

void API::createTable(const std::string& schemaname, const std::string& primarykey, const std::vector<AttrPtr>& attrs) {
    int idx = -1;
    for(int i = 0; i < attrs.size(); i++) {
        if(attrs[i]->name == primarykey) {
            idx = i;
            break;
        }
    }
    if(idx == -1) {
        throw AttributeError("[Error] Primary key '" + primarykey + "' doesn't exist.");
    }
    catalog->createTable(schemaname, idx, attrs);

    RecordPtr recordmanager = std::make_shared<RecordManager>(catalog->buffer, catalog, schemaname);
    RecordManager::recordbuf[schemaname] = recordmanager;
}

void API::dropTable(const std::string& schemaname) {
    catalog->dropTable(schemaname);
    if(RecordManager::recordbuf[schemaname]) {
        RecordManager::recordbuf[schemaname]->droptable();
    }
}

void API::inserter(const std::string& schemaname, Record& record) {
    SchemaPtr schema;
    if(RecordManager::recordexist(schemaname))
        schema = RecordManager::recordbuf[schemaname]->schema;
    else {
        RecordPtr recordmanager = std::make_shared<RecordManager>(catalog->buffer, catalog, schemaname);
        RecordManager::recordbuf[schemaname] = recordmanager;
        schema = recordmanager->schema;
    }

    if(record.size() != schema->attrs.size())
        throw TypeError("[Error] Column number error, except " + std::to_string(record.size()) + " got " + std::to_string(schema->attrs.size()) + ".");
    for(int i = 0; i < record.size(); i++) {
        if(record[i].type != schema->attrs[i]->vtype) {
            if(record[i].type == Type::INT && schema->attrs[i]->vtype == Type::FLOAT) {
                int* ptr = (int*)record[i].ptr;
                record[i].ptr = new float(*ptr);
                record[i].type = Type::FLOAT;
                delete ptr;
            }
            else if(record[i].type == Type::FLOAT && schema->attrs[i]->vtype == Type::INT) {
                float* ptr = (float*)record[i].ptr;
                record[i].ptr = new int(*ptr);
                record[i].type = Type::INT;
                delete ptr;
            }
            else {
                std::string msg = "[Error] Insert " + std::to_string(i) + "th data type mismatch, except " + tostr(schema->attrs[i]->vtype) + " got " + tostr(record[i].type);
                throw TypeError(msg);
            }
        }
        if(record[i].type == Type::CHAR && record[i].clen > schema->attrs[i]->clen) {
            std::string msg = "[Error] The string data(len < " + std::to_string(schema->attrs[i]->clen) + ") is too long.";
            throw TypeError(msg);
        }
    }

    RecordManager::recordbuf[schemaname]->inserter(record);
}

int API::deleter(const std::string& schemaname, const RawLimits& rawlimits) {
    SchemaPtr schema;
    if(RecordManager::recordexist(schemaname))
        schema = RecordManager::recordbuf[schemaname]->schema;
    else {
        RecordPtr recordmanager = std::make_shared<RecordManager>(catalog->buffer, catalog, schemaname);
        RecordManager::recordbuf[schemaname] = recordmanager;
        schema = recordmanager->schema;
    }
    
    Limits limit = schema->translimits(rawlimits);
    checklimits(schema, limit);

    int retval = 0;
    if(limit.empty() == true)
        retval = RecordManager::recordbuf[schemaname]->deleteall();
    else
        retval = RecordManager::recordbuf[schemaname]->deleter(limit);
    return retval;
}

std::vector<Record> API::selecter(const std::string& schemaname, const std::vector<std::string>& attrs, const RawLimits& rawlimits) {
    SchemaPtr schema;
    if(RecordManager::recordexist(schemaname))
        schema = RecordManager::recordbuf[schemaname]->schema;
    else {
        RecordPtr recordmanager = std::make_shared<RecordManager>(catalog->buffer, catalog, schemaname);
        RecordManager::recordbuf[schemaname] = recordmanager;
        schema = recordmanager->schema;
    }
    
    Limits limit = schema->translimits(rawlimits);
    checklimits(schema, limit);
    
    std::vector<Record> retval;
    std::vector<int> idxs;
    RecordPtr tmpptr = RecordManager::recordbuf[schemaname];
    if(attrs.empty()) {
        for(int i = 0; i < schema->attrs.size(); i++)
            idxs.push_back(i);
    }
    else {
        for(auto& e : attrs) {
            int idx = tmpptr->schema->getidx(e);
            if(idx == -1) throw AttributeError("[Error] Attribute '" + e + "' doesn't exist.");
            else idxs.push_back(idx);
        }
    }
    retval = tmpptr->project(tmpptr->selecter(limit), idxs);
    return retval;
}

void API::execfile(const std::string& filename) {
    interpreter::Driver* driver = new interpreter::Driver();
    try {
        try {
            driver->Parse(filename);
        }
        catch(BasicError &e) {
            std::cout << "Error occur" << std::endl;
            std::cout << e.msg << std::endl;
            return ;
        }
    }
    catch (std::logic_error &e) {
        return ;
    }
}

bool API::checklimits(const SchemaPtr& schema, const Limits& limit) {
    for(auto& e : limit) {
        if(e.val.type == schema->attrs[e.attridx]->vtype)
            continue;
        throw AttributeError("[Error] Constraint on '" + schema->attrs[e.attridx]->name + "' has an unmatched value type.");
    }
    return true;
}

void printline(const std::vector<int>& width) {
    std::cout << "+";
    for(auto& e : width) {
        for(int i = -1; i <= e; i++)
            std::cout << "-";
        std::cout << "+";
    }
    std::cout << std::endl;
}
void printval(const std::string& val, const int& maxlen) {
    std::cout << " " << val;
    for(int i = 0; i < maxlen - val.size(); i++)
        std::cout << " ";
    std::cout << " ";
}

void API::displaymsg(const std::string& schemaname, const std::vector<Record>& records, const std::vector<std::string>& attrs) {
    SchemaPtr schema;
    if(RecordManager::recordexist(schemaname))
        schema = RecordManager::recordbuf[schemaname]->schema;
    else {
        RecordPtr recordmanager = std::make_shared<RecordManager>(catalog->buffer, catalog, schemaname);
        RecordManager::recordbuf[schemaname] = recordmanager;
        schema = recordmanager->schema;
    }

    std::vector<int> idxs;
    RecordPtr tmpptr = RecordManager::recordbuf[schemaname];
    if(attrs.empty()) {
        for(int i = 0; i < schema->attrs.size(); i++)
            idxs.push_back(i);
    }
    else {
        for(auto& e : attrs) {
            int idx = tmpptr->schema->getidx(e);
            if(idx == -1) throw AttributeError("[Error] Attribute '" + e + "' doesn't exist.");
            else idxs.push_back(idx);
        }
    }

    std::vector<int> width;
    
    for(int i = 0; i < idxs.size(); i++) {
        int val = schema->attrs[idxs[i]]->name.size();
        for(auto& e : records) {
            switch (e[i].type) {
                case Type::INT :
                    val = std::max(val, (int)std::to_string(*(int*)e[i].ptr).size());
                    break;
                case Type::FLOAT :
                    val = std::max(val, (int)std::to_string(*(float*)e[i].ptr).size());
                    break;
                case Type::CHAR :
                    val = std::max(val, (int)std::string((char*)e[i].ptr).size());
                    break;
                default:
                    break;
            }
        }
        width.push_back(val);
    }

    printline(width);
    std::cout << "|";
    for(int i = 0; i < width.size(); i++) {
        printval(schema->attrs[idxs[i]]->name, width[i]);
        std::cout << "|";
    }
    std::cout << std::endl;
    printline(width);
    for(auto& e : records) {
        std::cout << "|";
        for(int i = 0; i < width.size(); i++) {
            printval(e[i].tostr(), width[i]);
            std::cout << "|";
        }
        std::cout << std::endl;
        printline(width);
    }
    std::cout << std::endl;
}