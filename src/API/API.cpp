#include "API.hpp"
#include "../Interpreter/Driver.hpp"

// API::API(const CatalogPtr& catalog) {
//     this->catalog = catalog;
// }
// API::~API() {

// }

CatalogPtr API::catalog;

void API::createTable(const std::string &schemaname, const std::string &primarykey, const std::vector<AttrPtr> &attrs) {
    if (catalog->schema_exist(schemaname)) {
        throw CatalogError("[Error] Schema " + schemaname + " exist.");
    }
    if (catalog->index_exist(schemaname)) {
        throw CatalogError("[Error] '" + schemaname + "' is already a index name.");
    }
    int idx = -1;
    for (int i = 0; i < attrs.size(); i++) {
        if (attrs[i]->name == primarykey) {
            idx = i;
            break;
        }
    }
    if (idx == -1) {
        throw AttributeError("[Error] Primary key '" + primarykey + "' doesn't exist.");
    }
    catalog->createTable(schemaname, idx, attrs);

    RecordPtr recordmanager = std::make_shared<RecordManager>(catalog->buffer, catalog->schemas[schemaname]);
    RecordManager::recordbuf[schemaname] = recordmanager;
}

void API::dropTable(const std::string &schemaname) {
    catalog->dropTable(schemaname);
    if (RecordManager::recordbuf[schemaname]) {
        RecordManager::recordbuf[schemaname]->droptable();
    }
    // drop all the indices associate with the table
    for (auto &a : catalog->schema2indexes[schemaname]) {
        catalog->dropIndex(a);
    }
}

void API::createIndex(const std::string &indexname, const std::string &schemaname, const std::string &columname) {
    SchemaPtr schema = catalog->schemas[schemaname];
    if (catalog->schema_exist(schemaname) == false)
        throw SchemaError("[Error] Schema '" + schemaname + "' doesn't exist.");
    if (catalog->schema_exist(indexname))
        throw SchemaError("[Error] Duplicate name of '" + indexname + "'.");
    if (catalog->index_exist(indexname))
        throw SchemaError("[Error] Index '" + indexname + "' already exist.");
    int idx = -1;
    for (int i = 0; i < schema->attrs.size(); i++) {
        if (schema->attrs[i]->name == columname) {
            if (schema->attrs[i]->unique == 0)
                throw AttributeError("[Error] Must create index on unique attribute.");
            idx = i;
            break;
        }
    }
    if (idx == -1) throw AttributeError("[Error] Column '" + columname + "' doesn't exist.");

    catalog->createIndex(indexname, schemaname, columname);
}

void API::dropIndex(const std::string &indexname) {
    catalog->dropIndex(indexname);
}

void API::inserter(const std::string &schemaname, Record &record) {
    SchemaPtr schema;
    if (RecordManager::recordexist(schemaname))
        schema = RecordManager::recordbuf[schemaname]->schema;
    else {
        RecordPtr recordmanager = std::make_shared<RecordManager>(catalog->buffer, catalog->schemas[schemaname]);
        RecordManager::recordbuf[schemaname] = recordmanager;
        schema = recordmanager->schema;
    }

    if (record.size() != schema->attrs.size())
        throw TypeError("[Error] Column number error, except " + std::to_string(record.size()) + " got " +
                        std::to_string(schema->attrs.size()) + ".");
    for (int i = 0; i < record.size(); i++) {
        if (record[i].type != schema->attrs[i]->vtype) {
            if (record[i].type == Type::INT && schema->attrs[i]->vtype == Type::FLOAT) {
                int *ptr = (int *) record[i].ptr;
                record[i].ptr = new float(*ptr);
                record[i].type = Type::FLOAT;
                delete ptr;
            } else if (record[i].type == Type::FLOAT && schema->attrs[i]->vtype == Type::INT) {
                float *ptr = (float *) record[i].ptr;
                record[i].ptr = new int(*ptr);
                record[i].type = Type::INT;
                delete ptr;
            } else {
                std::string msg = "[Error] Insert " + std::to_string(i) + "th data type mismatch, except " +
                                  tostr(schema->attrs[i]->vtype) + " got " + tostr(record[i].type);
                throw TypeError(msg);
            }
        }
        if (record[i].type == Type::CHAR && record[i].clen > schema->attrs[i]->clen) {
            std::string msg =
                    "[Error] The string data(len < " + std::to_string(schema->attrs[i]->clen) + ") is too long.";
            throw TypeError(msg);
        }
    }
    RecordManager::recordbuf[schemaname]->inserter(record);
    // insert to index
    if (catalog->schema2indexes.find(schema->header.name) != catalog->schema2indexes.end()) {
        for (const auto &a : catalog->schema2indexes[std::string(schema->header.name)]) {
            IndexPtr indexPtr = catalog->name2index[a];
            auto temp = RecordManager::recordbuf[schemaname]->header.recordstart;
            indexPtr->insertOne(temp + sizeof(unsigned long long) + schema->name2offset[indexPtr->columnName], temp);
        }
    }
}

int API::deleter(const std::string &schemaname, const RawLimits &rawlimits) {
    SchemaPtr schema;
    if (RecordManager::recordexist(schemaname))
        schema = RecordManager::recordbuf[schemaname]->schema;
    else {
        RecordPtr recordmanager = std::make_shared<RecordManager>(catalog->buffer, catalog->schemas[schemaname]);
        RecordManager::recordbuf[schemaname] = recordmanager;
        schema = recordmanager->schema;
    }

    Limits limit = schema->translimits(rawlimits);
    checklimits(schema, limit);


    int retval = 0;
    if (limit.empty()) {
        retval = RecordManager::recordbuf[schemaname]->deleteall();
        // drop all the indices associate with the table
        for (auto &a : catalog->schema2indexes[schemaname]) {
            catalog->dropIndex(a);
        }
    } else {
        retval = RecordManager::recordbuf[schemaname]->deleter(limit);
        // delete all the records in indices
        for (auto &a : catalog->schema2indexes[schemaname]) {
            catalog->name2index[a]->deleter(limit);
        }
    }
    return retval;
}

std::vector<Record>
API::selecter(const std::string &schemaname, const std::vector<std::string> &attrs, const RawLimits &rawlimits) {
    if (catalog->index_exist(schemaname))
        return selectbyindex(schemaname, attrs, rawlimits);
    else
        return selectbydefault(schemaname, attrs, rawlimits);
}

std::vector<Record>
API::selectbydefault(const std::string &schemaname, const std::vector<std::string> &attrs, const RawLimits &rawlimits) {
    if (!catalog->schema_exist(schemaname)) {
        throw CatalogError("[Error] Schema '" + schemaname + "' doesn't exist.");
    }
    SchemaPtr schema;
    if (RecordManager::recordexist(schemaname))
        schema = RecordManager::recordbuf[schemaname]->schema;
    else {
        RecordPtr recordmanager = std::make_shared<RecordManager>(catalog->buffer, catalog->schemas[schemaname]);
        RecordManager::recordbuf[schemaname] = recordmanager;
        schema = recordmanager->schema;
    }

    Limits limit = schema->translimits(rawlimits);
    checklimits(schema, limit);

    std::vector<int> idxs;
    RecordPtr tmpptr = RecordManager::recordbuf[schemaname];
    if (attrs.empty()) {
        for (int i = 0; i < schema->attrs.size(); i++)
            idxs.push_back(i);
    } else {
        for (auto &e : attrs) {
            int idx = tmpptr->schema->getidx(e);
            if (idx == -1) throw AttributeError("[Error] Attribute '" + e + "' doesn't exist.");
            else idxs.push_back(idx);
        }
    }

    std::vector<Record> retval = RecordManager::project(tmpptr->selecter(limit), idxs);
    return retval;
}

std::vector<Record>
API::selectbyindex(const std::string &indexname, const std::vector<std::string> &attrs, const RawLimits &rawlimits) {
    IndexPtr index = catalog->name2index[indexname];
    SchemaPtr schema = index->schema;

    Limits limit = schema->translimits(rawlimits);
    int indexidx = schema->getidx(index->columnName);
    bool flag = false;
    std::vector<Constraint> indexlim;
    for (auto &e : limit) {
        if (indexidx == e.attridx) {
            flag = true; // contains constraints on the indexed column
            indexlim.push_back(e);
        }
    }
    if (!flag) {
        std::cerr << "[WARNING] Must contains constraints on the indexed `" + index->columnName +
                     "` column to speed up query." << std::endl;
        // TODO: remove
        std::cout << "Selecting from <- " << schema->header.name << std::endl;
        return selectbydefault(std::string(schema->header.name), attrs, rawlimits);
    } else {
        // find & remove the constraints on left or right to get the records
        // leftVal [<, <=] val [<, <=] rightVal or val == someVal
        // the rest is then filtered out
        bool wl, leq, wr, req, eq;
        Value l, r, eqval;
        std::vector<unsigned long long> result;
        unsigned long long temp;
        for (auto &lim : indexlim) { // column is placed on the left of limit's operator
            if (lim.op == Operator::EQ) {
                eq = true;
                eqval = lim.val;
                break;
            } else if (lim.op == Operator::GT || lim.op == Operator::GEQ) {
                if (!wl) {
                    l = lim.val;
                    wl = true;
                } else { // already sure there is lower bound
                    if ((Value::valcmp(lim.val, l) == 0 && lim.op == Operator::GT)
                        || (Value::valcmp(lim.val, l) > 0)) {
                        l = lim.val;
                        leq = lim.op == Operator::GEQ;
                    }
                }
            } else if (lim.op == Operator::LT || lim.op == Operator::LEQ) {
                if (!wr) {
                    r = lim.val;
                    wr = true;
                } else { // already sure there is upper bound
                    if ((Value::valcmp(lim.val, r) == 0 && lim.op == Operator::LT)
                        || (Value::valcmp(lim.val, r) < 0)) {
                        r = lim.val;
                        req = lim.op == Operator::LEQ;
                    }
                }
            }
        }
        // take the find results and translate them into records
        if (eq) {
            if ((temp = index->findOne(eqval)) != 0) {
                result.push_back(temp);
            }
        } else {
            result = index->findByRange(
                    wl, leq, l,
                    wr, req, r
            );
        }
        // filter the records with the rest of the constraints
        return index->translateAndFilter(result, limit);
    }

    //transfer limits

    std::vector<int> idxs;
    if (attrs.empty()) {
        for (int i = 0; i < schema->attrs.size(); i++)
            idxs.push_back(i);
    } else {
        for (auto &e : attrs) {
            int idx = schema->getidx(e);
            if (idx == -1) throw AttributeError("[Error] Attribute '" + e + "' doesn't exist.");
            else idxs.push_back(idx);
        }
    }

    std::vector<Record> retval;
    // retval = RecordManager::project(index->selecter(limit), idxs);
    //get retval by limits
    return retval;
}

void API::execfile(const std::string &filename) {
    interpreter::Driver *driver = new interpreter::Driver();
    try {
        try {
            driver->Parse(filename);
        }
        catch (BasicError &e) {
            std::cout << "Error occur" << std::endl;
            std::cout << e.msg << std::endl;
            return;
        }
    }
    catch (std::logic_error &e) {
        return;
    }
}

bool API::checklimits(const SchemaPtr &schema, const Limits &limit) {
    for (auto &e : limit) {
        if (e.val.type == schema->attrs[e.attridx]->vtype)
            continue;
        throw AttributeError(
                "[Error] Constraint on '" + schema->attrs[e.attridx]->name + "' has an unmatched value type.");
    }
    return true;
}

void printline(const std::vector<int> &width) {
    std::cout << "+";
    for (auto &e : width) {
        for (int i = -1; i <= e; i++)
            std::cout << "-";
        std::cout << "+";
    }
    std::cout << std::endl;
}

void printval(const std::string &val, const int &maxlen) {
    std::cout << " " << val;
    for (int i = 0; i < maxlen - val.size(); i++)
        std::cout << " ";
    std::cout << " ";
}

void API::displaymsg(const std::string &schemaname, const std::vector<Record> &records,
                     const std::vector<std::string> &attrs) {
    // if its a index name

    SchemaPtr schema;
    if (RecordManager::recordexist(schemaname))
        schema = RecordManager::recordbuf[schemaname]->schema;
    else if (catalog->index_exist(schemaname)){
        schema = catalog->name2index[schemaname]->schema;
    }else {
        RecordPtr recordmanager = std::make_shared<RecordManager>(catalog->buffer, catalog->schemas[schemaname]);
        RecordManager::recordbuf[schemaname] = recordmanager;
        schema = recordmanager->schema;
    }

    std::vector<int> idxs;
    RecordPtr tmpptr = RecordManager::recordbuf[schemaname];
    if (attrs.empty()) {
        for (int i = 0; i < schema->attrs.size(); i++)
            idxs.push_back(i);
    } else {
        for (auto &e : attrs) {
            int idx = tmpptr->schema->getidx(e);
            if (idx == -1) throw AttributeError("[Error] Attribute '" + e + "' doesn't exist.");
            else idxs.push_back(idx);
        }
    }

    std::vector<int> width;

    for (int i = 0; i < idxs.size(); i++) {
        int val = schema->attrs[idxs[i]]->name.size();
        for (auto &e : records) {
            switch (e[i].type) {
                case Type::INT :
                    val = std::max(val, (int) std::to_string(*(int *) e[i].ptr).size());
                    break;
                case Type::FLOAT :
                    val = std::max(val, (int) std::to_string(*(float *) e[i].ptr).size());
                    break;
                case Type::CHAR :
                    val = std::max(val, (int) std::string((char *) e[i].ptr).size());
                    break;
                default:
                    break;
            }
        }
        width.push_back(val);
    }

    printline(width);
    std::cout << "|";
    for (int i = 0; i < width.size(); i++) {
        printval(schema->attrs[idxs[i]]->name, width[i]);
        std::cout << "|";
    }
    std::cout << std::endl;
    printline(width);
    for (auto &e : records) {
        std::cout << "|";
        for (int i = 0; i < width.size(); i++) {
            printval(e[i].tostr(), width[i]);
            std::cout << "|";
        }
        std::cout << std::endl;
        printline(width);
    }
    std::cout << std::endl;
}