//
// Created by Henry Little on 2019-06-08.
//

#include "IndexManager.hpp"

using FilePos = std::pair<int, int>;
auto CalcFilePos = [](const unsigned long long& pos) {return std::make_pair(pos / BLOCK_SIZE, pos % BLOCK_SIZE);};

IndexManager::IndexManager(const BufferPtr &recordBuffer, const SchemaPtr &schema, const std::string colName) {
    
    this->recordBuffer = recordBuffer;
    this->columnName = colName;
    this->tableName = tableName;
    this->schema = schema;

    std::string filename = std::string(FILENAME_PREFIX) + "index-" + colName + "-" + std::string(schema->header.name);

    if (FileManager::filexist(filename.c_str()) == false) {
        FileManager::createfile(filename.c_str());
        this->indexFile = std::make_shared<FileManager>(filename.c_str());
        FileManager::filebuf[filename] = indexFile;
        assert(indexFile->allocblock() == 0);
        // TODO: write the file header
    } else {
        if(FileManager::filebuf[filename].expired() == false) {
            indexFile = FileManager::filebuf[filename].lock();
        } else {
            indexFile = std::make_shared<FileManager>(filename.c_str()); // make_shared will call the constructor of the FileManger
            FileManager::filebuf[filename] = indexFile;
        }
    }
    
    
}

Value IndexManager::getVal(long long roffset) {
    FilePos pos = CalcFilePos(roffset);
    BlockPtr tempblk = recordBuffer->getblock(MakeID(recordFile, pos.first));
}