#ifndef minisql_config_header
#define minisql_config_header

#define BLOCK_SIZE 0x4000
#define MAX_CACHE_NUMBER 0x1000
#define MAX_NAME_LEN 0x40
#define DELETE_TAG 0X8000000000000000LL
#define DELETE_MASK 0X0FFFFFFFFFFFFFFFLL

#define FILENAME_PREFIX "files/"
#define CATALOG_NAME "mystery"

#endif