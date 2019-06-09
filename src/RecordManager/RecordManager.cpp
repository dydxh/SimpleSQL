#include <string>
#include <cassert>
#include "RecordManager.hpp"
#include "../utils/config.hpp"
#include "../Type/Value.hpp"

using FilePos = std::pair<int, int>;
auto CalcFilePos = [](const int& pos) {return std::make_pair(pos / BLOCK_SIZE, pos % BLOCK_SIZE);};

RecordManager::RecordManager(const BufferPtr& buffer, const CatalogPtr& catalog, std::string schemaname) {
    if(catalog->schema_exist(schemaname) == false) {
        throw SchemaError("[Error] Schema " + schemaname + " doesn't exist." );
    }
    this->schema = catalog->schemas[schemaname];
    this->buffer = buffer;
    std::string filename = FILENAME_PREFIX + schemaname + "-" + CATALOG_NAME;

    if(FileManager::filexist(filename.c_str()) == false) {
        FileManager::createfile(filename.c_str());
        file = std::make_shared<FileManager>(filename.c_str());
        FileManager::filebuf[filename] = file;
        assert(file->allocblock() == 0);
        RecordFileHeader tmpheader{2, 0, 0LL, 1LL * BLOCK_SIZE};
        BlockPtr tmpblk = buffer->getblock(MakeID(file, 0));
        tmpblk->setoffset(0);
        tmpblk->write(&tmpheader, sizeof(tmpheader));
        assert(file->allocblock() == 1);
    }
    else {
        if(FileManager::filebuf[filename].expired() == false)
            file = FileManager::filebuf[filename].lock();
        else {
            file = std::make_shared<FileManager>(filename.c_str());
            FileManager::filebuf[filename] = file;
        }
    }

    readheader();
}

RecordManager::~RecordManager() {

}

void RecordManager::readheader() {
    BlockPtr tmpblk = buffer->getblock(MakeID(file, 0));
    tmpblk->setoffset(0);
    tmpblk->read(&(this->header), sizeof(header));
}

int RecordManager::inserter(const Record& record) {
    FilePos pos = CalcFilePos(header.freestart);
    BlockPtr tmpblk = buffer->getblock(MakeID(file, pos.first));
    tmpblk->setoffset(pos.second);

    int a = record.size();
    int b = schema->attrs.size();
    if(record.size() != schema->attrs.size())
        throw TypeError("[Error] Column number error. Except " + std::to_string(record.size()) + " got " + std::to_string(schema->attrs.size()) + ".");
    for(int i = 0; i < record.size(); i++) {
        if(record[i].type != schema->attrs[i]->vtype) {
            std::string msg = "[Error] Insert " + std::to_string(i) + "th data type mismatch. Except " + tostr(schema->attrs[i]->vtype) + " got " + tostr(record[i].type);
            throw TypeError(msg);
        }
        if(record[i].type == Type::CHAR && record[i].clen > schema->attrs[i]->clen) {
            std::string msg = "[Error] The " + std::to_string(i) + "th data is too long.";
            throw TypeError(msg);
        }
    }

    int recordsize = sizeof(unsigned long long);
    tmpblk->write(&header.recordstart, sizeof(unsigned long long));
    for(int i = 0; i < record.size(); i++) {
        tmpblk->write(record[i].ptr, schema->attrs[i]->size());
        recordsize += schema->attrs[i]->size();
    }
    header.recordstart = header.freestart;
    header.recordnumber += 1;
    if(pos.second + (recordsize << 1) >= BLOCK_SIZE) {
        header.freestart = (header.blocknumber++) * BLOCK_SIZE;
        file->allocblock();
    }
    else
        header.freestart += recordsize;
    tmpblk = buffer->getblock(MakeID(file, 0));
    tmpblk->setoffset(0);
    tmpblk->write(&(this->header), sizeof(this->header));
    return 0;
}
