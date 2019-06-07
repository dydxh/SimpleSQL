#ifndef minisql_errormanager_header
#define minisql_errormanager_header

#include <string>

class BasicError {
public:
    std::string msg;
    BasicError(const std::string& msg) : msg(msg) {}
};

class FileError : public BasicError {
public:
    FileError(const std::string& msg) : BasicError(msg) {}    
};

class OpenFileError : public FileError {
public:
    OpenFileError(const std::string& msg) : FileError(msg) {}    
};

class BlockError : public BasicError {
public:
    BlockError(const std::string& msg) : BasicError(msg) {}
};

class OutofBoundary : public BlockError {
public:
    OutofBoundary(const std::string& msg) : BlockError(msg) {}
};

#endif