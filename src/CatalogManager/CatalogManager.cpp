#include <memory>
#include <cassert>
#include <algorithm>
#include "../utils/config.hpp"
#include "CatalogManager.hpp"
#include "Schema.hpp"
#include "../utils/ErrorManager.hpp"
#include "../FileManager/FileManager.hpp"

CatalogManager::CatalogManager(const BufferPtr &bptr, const std::string name) {
    std::string filename = FILENAME_PREFIX + name + ".catalog";
    buffer = bptr;
    this->name = name;

    if (FileManager::filexist(filename.c_str()) == false) {
        FileManager::createfile(filename.c_str());
        file = std::make_shared<FileManager>(filename.c_str());
        FileManager::filebuf[filename] = file;
        assert(file->allocblock() == 0);
        CatalogHeader tmpheader{1, 0, 0, 0};
        BlockPtr tmpblk = buffer->getblock(MakeID(file, 0));
        tmpblk->setoffset(0);
        tmpblk->write(&tmpheader, sizeof(tmpheader));
    } else {
        if (FileManager::filebuf[filename].expired() == false)
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
    while (curblk) {
        BlockPtr tmpblk = buffer->getblock(MakeID(file, curblk));
        tmpblk->setoffset(0);
        SchemaPtr tmpschema = std::make_shared<Schema>(this->file, this->buffer);

        tmpblk->read(&(tmpschema->header), sizeof(tmpschema->header));
        if (tmpschema->header.deleted) {
            curblk = tmpschema->header.nextblk;
            continue;
        }

        char tmpname[MAX_NAME_LEN];
        int typeval;
        for (int i = 0; i < tmpschema->header.attrcnt; i++) {
            tmpblk->read(&(typeval), sizeof(typeval));
            tmpblk->read(tmpname, MAX_NAME_LEN);
            AttrPtr tmpptr = std::make_shared<Attribute>(std::string(tmpname), typeval);

            tmpschema->attrs.push_back(tmpptr);
            tmpschema->name2attrs[tmpptr->name] = tmpptr;
            
            if(i == 0) {
                tmpschema->idx2offset.push_back(0);
            }
            int offset = tmpptr->size();
            if (i != 0) {
                offset += tmpschema->idx2offset[i];
            }
            tmpschema->idx2offset.push_back(offset);
            tmpschema->name2offset[tmpptr->name] = tmpschema->idx2offset[i];
        }

        nextblk = tmpschema->header.nextblk;
        schemas[std::string(tmpschema->header.name)] = tmpschema;
        name2offset[std::string(tmpschema->header.name)] = curblk;
        schema2indexes[std::string(tmpschema->header.name)] = std::vector<std::string>();
        curblk = nextblk;
    }
    curblk = header.indexblk;
    while (curblk) {
        BlockPtr tmpblk = buffer->getblock(MakeID(file, curblk));
        tmpblk->setoffset(0);
        IndexHeader tmpheader;

        tmpblk->read(&(tmpheader), sizeof(tmpheader));
        if (tmpheader.deleted) {
            curblk = tmpheader.nextblk;
            continue;
        }
        IndexPtr tmpptr = std::make_shared<IndexManager>(
                this->buffer, tmpheader.indexname,
                schemas[tmpheader.schemaname], tmpheader.columnname,
                tmpheader);

        nextblk = tmpheader.nextblk;
        name2index[std::string(tmpheader.indexname)] = tmpptr;
        name2offset[std::string(tmpheader.indexname)] = curblk;
        schema2indexes[std::string(tmpheader.schemaname)].push_back(tmpheader.indexname);
        curblk = nextblk;
    }
}

void
CatalogManager::createTable(const std::string &schemaname, const int &idx, const std::vector<AttrPtr> &attributes) {
    SchemaPtr tmpschema = std::make_shared<Schema>(this->file, this->buffer);

    memcpy(tmpschema->header.name, schemaname.c_str(), MAX_NAME_LEN);
    tmpschema->header.deleted = 0;
    tmpschema->header.attrcnt = attributes.size();
    tmpschema->header.primaryidx = idx;
    for (auto &e : attributes) {
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
    tmpschema->attrs[idx]->unique = 1;
    char tmpbuf[MAX_NAME_LEN];

    int i = 0;
    for (auto &e : tmpschema->attrs) {
        *((int *) tmpbuf) = e->getattr();
        tmpblk->write(&tmpbuf, sizeof(int));
        memcpy(tmpbuf, e->name.c_str(), MAX_NAME_LEN);
        tmpblk->write(tmpbuf, MAX_NAME_LEN);

        if(i == 0) {
            tmpschema->idx2offset.push_back(0);
        }
        int offset = e->size();
        if (i != 0) {
            offset += tmpschema->idx2offset[i];
        }
        tmpschema->idx2offset.push_back(offset);
        tmpschema->name2offset[e->name] = tmpschema->idx2offset[i];
        i++;
    }
    schemas[schemaname] = tmpschema;
    name2offset[schemaname] = this->header.schemablk;
}

void CatalogManager::dropTable(const std::string &schemaname) {
    if (schema_exist(schemaname) == false) {
        throw CatalogError("Schema " + schemaname + " doesn't exist.");
    }
    int offset = name2offset[schemaname], tmpval = 1;
    BlockPtr tmpblk = buffer->getblock(MakeID(file, offset));
    tmpblk->setoffset(4);
    tmpblk->write(&tmpval, sizeof(tmpval));
    schemas.erase(schemaname);
    name2offset.erase(schemaname);
    for (auto &e : schema2indexes[schemaname])
        dropIndex(e);
    schema2indexes.erase(schemaname);
}

void
CatalogManager::createIndex(const std::string &indexname, const std::string &schemaname, const std::string &columname) {
    //modify catalog file header and write index header to file
    IndexHeader tmpheader;
    tmpheader.nextblk = this->header.indexblk;
    tmpheader.deleted = 0;
    tmpheader.treeRoot = 0;
    memcpy(tmpheader.indexname, indexname.c_str(), MAX_NAME_LEN);
    memcpy(tmpheader.schemaname, schemaname.c_str(), MAX_NAME_LEN);
    memcpy(tmpheader.columnname, columname.c_str(), MAX_NAME_LEN);

    IndexPtr indexptr = std::make_shared<IndexManager>(
            this->buffer, indexname, schemas[schemaname],
            columname, tmpheader);
    indexptr->buildIndex();
    tmpheader.treeRoot = indexptr->header.treeRoot;
    this->header.indexblk = this->header.blocknumber++;
    file->allocblock();

    BlockPtr tmpblk = buffer->getblock(MakeID(file, 0));
    tmpblk->setoffset(0);
    tmpblk->write(&(this->header), sizeof(this->header));

    tmpblk = buffer->getblock(MakeID(file, this->header.indexblk));
    tmpblk->setoffset(0);
    tmpblk->write(&tmpheader, sizeof(tmpheader));

    name2index[indexname] = indexptr;
    name2offset[indexname] = this->header.indexblk;
    schema2indexes[schemaname].push_back(indexname);
}

void CatalogManager::dropIndex(const std::string &indexname) {
    if (!index_exist(indexname)) {
        throw CatalogError("Index '" + indexname + "' doesn't exist.");
    }
    int offset = name2offset[indexname], tmpval = 1;
    BlockPtr tmpblk = buffer->getblock(MakeID(file, offset));
    tmpblk->setoffset(4);
    tmpblk->write(&tmpval, sizeof(tmpval));
    name2index[indexname]->indexFile->setdelete();

    std::string schemaname = name2index[indexname]->schema->header.name;
    auto aa = std::find(schema2indexes[schemaname].begin(), schema2indexes[schemaname].end(), indexname);

    schema2indexes[schemaname].erase(aa);
    name2offset.erase(indexname);
    name2index.erase(indexname);
}
