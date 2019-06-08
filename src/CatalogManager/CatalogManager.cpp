#include <memory>
#include <cassert>
#include "../utils/config.hpp"
#include "CatalogManager.hpp"
#include "Schema.hpp"

CatalogManager::CatalogManager(const BufferPtr& bptr, const std::string name) {
    std::string filename = FILENAME_PREFIX + name + ".catalog";
    buffer = bptr;
    this->name = name;

    if(FileManager::filexist(filename.c_str()) == false) {
        FileManager::createfile(filename.c_str());
        file = std::make_shared<FileManager>(filename.c_str());
        assert(file->allocblock() == 0);
        CatalogHeader tmpheader{1, 0, 0, 0};
        BlockPtr tmpblk = buffer->getblock(MakeID(file, 0));
        tmpblk->setoffset(0);
        tmpblk->write(&tmpheader, sizeof(tmpheader));
    }
    else {
        file = std::make_shared<FileManager>(filename.c_str());
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

        char tmpname[MAX_NAME_LEN]; int typeval;
        for(int i = 0; i < tmpschema->header.attrcnt; i++) {
            tmpblk->read(&(typeval), sizeof(typeval));
            tmpblk->read(tmpname, MAX_NAME_LEN);
            AttrPtr tmpptr = std::make_shared<Attribute>(std::string(tmpname), typeval);

            tmpschema->attrs.push_back(tmpptr);
            tmpschema->name2attrs[tmpptr->name] = tmpptr;
        }

        nextblk = tmpschema->header.nextblk;
        schemas[std::string(tmpschema->header.name)] = tmpschema;
        name2offset[std::string(tmpschema->header.name)] = curblk;
        curblk = nextblk;
    }
}

void CatalogManager::createTable(const std::string& schemaname, const int& idx, const std::vector<AttrPtr>& attributes) {
    if(schema_exist(schemaname)) {
        std::cout << "QAQ file exist." << std::endl;
        return ;
    }
    return ;
    SchemaPtr tmpschema = std::make_shared<Schema>(this->file, this->buffer);

    memcpy(tmpschema->header.name, schemaname.c_str(), MAX_NAME_LEN);
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
}

