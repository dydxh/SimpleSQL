#include <memory>
#include <cassert>
#include "../utils/config.hpp"
#include "CatalogManager.hpp"
#include "Schema.hpp"
#include "../utils/ErrorManager.hpp"
#include "../FileManager/FileManager.hpp"

CatalogManager::CatalogManager(const BufferPtr& bptr, const std::string name) {
    std::string filename = FILENAME_PREFIX + name + ".catalog";
    buffer = bptr;
    this->name = name;

    if(FileManager::filexist(filename.c_str()) == false) {
        FileManager::createfile(filename.c_str());
        file = std::make_shared<FileManager>(filename.c_str());
        FileManager::filebuf[filename] = file;
        assert(file->allocblock() == 0);
        CatalogHeader tmpheader{1, 0, 0, 0};
        BlockPtr tmpblk = buffer->getblock(MakeID(file, 0));
        tmpblk->setoffset(0);
        tmpblk->write(&tmpheader, sizeof(tmpheader));
    }
    else {
        if(FileManager::filebuf[filename].expired() == false)
            file = FileManager::filebuf[filename].lock();
        else {
            file = std::make_shared<FileManager>(filename.c_str());
            FileManager::filebuf[filename] = file;
        }
    }
    
    //read header
    readheader();
    readbody();
}

void CatalogManager::readheader() {
    BlockPtr tmpblk = buffer->getblock(MakeID(file, 0));
    tmpblk->setoffset(0);
    tmpblk->read(&(this->header), sizeof(header));
}

void CatalogManager::readbody() {
    int curblk = header.schemablk, nextblk;
    while(curblk) {
        BlockPtr tmpblk = buffer->getblock(MakeID(file, curblk));
        tmpblk->setoffset(0);
        SchemaPtr tmpschema = std::make_shared<Schema>(this->file, this->buffer);

        tmpblk->read(&(tmpschema->header), sizeof(tmpschema->header));
        if(tmpschema->header.deleted) {
            curblk = tmpschema->header.nextblk;
            continue;
        }

        char tmpname[MAX_NAME_LEN]; int typeval;
        for(int i = 0; i < tmpschema->header.attrcnt; i++) {
            tmpblk->read(&(typeval), sizeof(typeval));
            tmpblk->read(tmpname, MAX_NAME_LEN);
            AttrPtr tmpptr = std::make_shared<Attribute>(std::string(tmpname), typeval);

            tmpschema->attrs.push_back(tmpptr);
            tmpschema->name2attrs[tmpptr->name] = tmpptr;
            int offset = tmpptr->size();
            if(i != 0)
                offset += tmpschema->idx2offset[i - 1];
            tmpschema->idx2offset.push_back(offset);
            tmpschema->name2offset[tmpptr->name] = tmpschema->idx2offset[i];
        }

        nextblk = tmpschema->header.nextblk;
        schemas[std::string(tmpschema->header.name)] = tmpschema;
        name2offset[std::string(tmpschema->header.name)] = curblk;
        curblk = nextblk;
    }
}

void CatalogManager::createTable(const std::string& schemaname, const int& idx, const std::vector<AttrPtr>& attributes) {
    if(schema_exist(schemaname)) {
        throw CatalogError("Schema " + schemaname + " exist.");
    }
    SchemaPtr tmpschema = std::make_shared<Schema>(this->file, this->buffer);

    memcpy(tmpschema->header.name, schemaname.c_str(), MAX_NAME_LEN);
    tmpschema->header.deleted = 0;
    tmpschema->header.attrcnt = attributes.size();
    tmpschema->header.primaryidx = idx;
    for(auto& e : attributes) {
        tmpschema->attrs.push_back(e);
        tmpschema->name2attrs[e->name] = e;
    }

    tmpschema->header.nextblk = this->header.schemablk;
    this->header.schemablk = this->header.blocknumber++;
    file->allocblock();

    BlockPtr tmpblk = buffer->getblock(MakeID(file, 0));
    tmpblk->setoffset(0);
    tmpblk->write(&(this->header), sizeof(this->header));

    tmpblk = buffer->getblock(MakeID(file, this->header.schemablk));
    tmpblk->setoffset(0);
    tmpblk->write(&(tmpschema->header), sizeof(tmpschema->header));

    char tmpbuf[MAX_NAME_LEN];
    for(auto& e : tmpschema->attrs) {
        *((int *)tmpbuf) = e->getattr();
        tmpblk->write(&tmpbuf, sizeof(int));
        memcpy(tmpbuf, e->name.c_str(), MAX_NAME_LEN);
        tmpblk->write(tmpbuf, MAX_NAME_LEN);
    }
    schemas[schemaname] = tmpschema;
    name2offset[schemaname] = this->header.schemablk;
}

void CatalogManager::dropTable(const std::string& schemaname) {
    if(schema_exist(schemaname) == false) {
        throw CatalogError("Schema " + schemaname + " doesn't exist.");
    }
    int offset = name2offset[schemaname], tmpval = 1;
    BlockPtr tmpblk = buffer->getblock(MakeID(file, offset));
    tmpblk->setoffset(4);
    tmpblk->write(&tmpval, sizeof(tmpval));
    schemas.erase(schemaname);
    name2offset.erase(schemaname);
}
