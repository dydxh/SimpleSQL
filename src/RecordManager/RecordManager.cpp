#include <string>
#include <cassert>
#include "RecordManager.hpp"
#include "../utils/config.hpp"
#include "../Type/Value.hpp"

using FilePos = std::pair<int, int>;
auto CalcFilePos = [](const unsigned long long &pos) { return std::make_pair(pos / BLOCK_SIZE, pos % BLOCK_SIZE); };

std::map<std::string, std::shared_ptr<RecordManager>> RecordManager::recordbuf;

RecordManager::RecordManager(const BufferPtr &buffer, const SchemaPtr& schema) {
    this->schema = schema;
    this->buffer = buffer;
    this->schemaname = schemaname;
    std::string filename = FILENAME_PREFIX + schemaname + "-" + CATALOG_NAME;

    if (FileManager::filexist(filename.c_str()) == false) {
        FileManager::createfile(filename.c_str());
        file = std::make_shared<FileManager>(filename.c_str());
        FileManager::filebuf[filename] = file;
        assert(file->allocblock() == 0);
        RecordFileHeader tmpheader{2, 0, 0LL, 1LL * BLOCK_SIZE};
        BlockPtr tmpblk = buffer->getblock(MakeID(file, 0));
        tmpblk->setoffset(0);
        tmpblk->write(&tmpheader, sizeof(tmpheader));
        assert(file->allocblock() == 1);
    } else {
        if (FileManager::filebuf[filename].expired() == false)
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

bool RecordManager::recordexist(const std::string &schemaname) {
    if (recordbuf.find(schemaname) != recordbuf.end())
        return true;
    return false;
}

void RecordManager::droptable() {
    file->setdelete();
    RecordManager::recordbuf.erase(schemaname);
}

void RecordManager::readheader() {
    BlockPtr tmpblk = buffer->getblock(MakeID(file, 0));
    tmpblk->setoffset(0);
    tmpblk->read(&(this->header), sizeof(header));
}

void RecordManager::inserter(const Record &record) {
    FilePos pos = CalcFilePos(header.freestart);
    BlockPtr tmpblk = buffer->getblock(MakeID(file, pos.first));
    tmpblk->setoffset(pos.second);

    int recordsize = sizeof(unsigned long long);
    tmpblk->write(&header.recordstart, sizeof(unsigned long long));
    for (int i = 0; i < record.size(); i++) {
        tmpblk->write(record[i].ptr, schema->attrs[i]->size());
        recordsize += schema->attrs[i]->size();
    }
    header.recordstart = header.freestart;
    header.recordnumber += 1;
    if (pos.second + (recordsize << 1) >= BLOCK_SIZE) {
        header.freestart = (header.blocknumber++) * BLOCK_SIZE;
        file->allocblock();
    } else
        header.freestart += recordsize;
    tmpblk = buffer->getblock(MakeID(file, 0));
    tmpblk->setoffset(0);
    tmpblk->write(&(this->header), sizeof(this->header));
}

int RecordManager::deleteall() {
    int retval = header.recordnumber;
    header.recordnumber = 0;
    header.recordstart = 0;
    header.freestart = BLOCK_SIZE;
    BlockPtr tmpblk = buffer->getblock(MakeID(file, 0));
    tmpblk->setoffset(0);
    tmpblk->write(&this->header, sizeof(this->header));
    return retval;
}

std::vector<Record> RecordManager::deleter(const Limits &limit) {
    std::vector<Record> retval = std::vector<Record>();
    BlockPtr tmpblk;
    unsigned long long ptr = header.recordstart;
    while (ptr) {
        FilePos pos = CalcFilePos(ptr);
        tmpblk = buffer->getblock(MakeID(file, pos.first));
        tmpblk->setoffset(pos.second);
        tmpblk->read(&ptr, sizeof(unsigned long long));
        if (ptr & DELETE_TAG) {
            ptr &= DELETE_MASK;
            continue;
        }
        Record record;
        for (auto &e : schema->attrs) {
            Value val;
            val.type = e->vtype;
            if (val.type == Type::CHAR) {
                val.clen = e->clen;
                val.ptr = new char[val.clen];
            } else if (val.type == Type::INT) val.ptr = new int;
            else val.ptr = new float;
            tmpblk->read(val.ptr, e->size());
            record.emplace_back(std::move(val));
        }
        bool to_delete = true;
        for (auto &e : limit) {
            to_delete &= Constraint::valcmp(e.op, record[e.attridx], e.val);
            if (to_delete == false) break;
        }
        if (to_delete) {
            retval.push_back(record);
            unsigned long long newval = ptr | DELETE_TAG;
            tmpblk->setoffset(pos.second);
            tmpblk->write(&newval, sizeof(unsigned long long));
        }
    }
    header.recordnumber -= retval.size();
    tmpblk = buffer->getblock(MakeID(file, 0));
    tmpblk->setoffset(0);
    tmpblk->write(&this->header, sizeof(this->header));
    return retval;
}

std::vector<Record> RecordManager::selecter(const Limits& limit) {
    std::vector<Record> retval;
    BlockPtr tmpblk;
    unsigned long long ptr = header.recordstart;

    while (ptr) {
        FilePos pos = CalcFilePos(ptr);
        tmpblk = buffer->getblock(MakeID(file, pos.first));
        tmpblk->setoffset(pos.second);
        tmpblk->read(&ptr, sizeof(unsigned long long));
        if (ptr & DELETE_TAG) {
            ptr &= DELETE_MASK;
            continue;
        }
        Record record;
        for (auto &e : schema->attrs) {
            Value val;
            val.type = e->vtype;
            if (val.type == Type::CHAR) {
                val.clen = e->clen;
                val.ptr = new char[val.clen];
            } else if (val.type == Type::INT) val.ptr = new int;
            else val.ptr = new float;
            tmpblk->read(val.ptr, e->size());
            record.emplace_back(std::move(val));
        }
        bool reserved = true;
        for (auto &e : limit) {
            reserved &= Constraint::valcmp(e.op, record[e.attridx], e.val);
            if (!reserved) break;
        }
        if (reserved) retval.emplace_back(std::move(record));
    }
    return retval;
}

Value RecordManager::getVal(const char *columnName, unsigned long long roffset) {
    FilePos pos = CalcFilePos(roffset);
    BlockPtr tempblk = buffer->getblock(MakeID(file, pos.first));
    int size = schema->name2attrs[std::string(columnName)]->size();
    void *ptr = malloc(size);
    tempblk->setoffset(pos.second);
    tempblk->read(ptr, size);
    Value val;
    val.type = schema->name2attrs[std::string(columnName)]->vtype;
    val.ptr = ptr;
    if (val.type == Type::CHAR) {
        val.clen = schema->name2attrs[std::string(columnName)]->clen;
    }
    return val;
}

std::vector<Record> RecordManager::project(const std::vector<Record> &records, const std::vector<int> &idx) {
    std::vector<Record> retval;
    for (auto &record : records) {
        Record val;
        for (auto &pos : idx)
            val.push_back(record[pos]);
        retval.push_back(val);
    }
    return retval;
}
