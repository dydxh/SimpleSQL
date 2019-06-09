#include "RecordManager.hpp"

RecordManager::RecordManager(const BufferPtr& buffer, const std::string schemaname) {
    this->buffer = buffer;
    
}

RecordManager::~RecordManager() {

}

int RecordManager::inserter(const Record& record) {

    return 0;
}
