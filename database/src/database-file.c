//#include "../include/aggregator/private/database.h"
//
//struct BlockHeader *headerBlock = NULL;
//struct DatabaseHeader *header = NULL;
//
//static int32_t getHashMap(struct BlockHeader **bh) {
//    struct BlockCoordinate coordinates = header->tableMapPage;
//    checkForErr(getBlockByOffsetAndSize(bh, coordinates.offset, coordinates.size));
//    return SUCCESS;
//}
//
//static int32_t getTableAndReturnBlock(struct BlockHeader **bh, const void *name) {
//    struct BlockHeader *blockWithMap = NULL;
//    checkForErr(getHashMap(&blockWithMap))
//    int32_t err = getTableByNameAndReturnBlock(bh, getBlockData(blockWithMap), name);
//    unmapPage(blockWithMap);
//    return err;
//}
//
//int32_t getTable(struct TableHeader **table, const char *name) {
//    struct BlockHeader *block;
//    checkForErr(getTableAndReturnBlock(&block, name));
//    *table = convertFileNodeToTableHeader(getBlockData(block));
//    return SUCCESS;
//}
//
//int32_t freeTable(struct TableHeader *tableHeader) {
//    if (tableHeader != NULL) {
//        struct BlockHeader *tableBlock = convertTableHeaderToBlockHeader(tableHeader);
//        unmapPage(tableBlock);
//    }
//    return SUCCESS;
//}
//
//int32_t createTable(const char *name, int32_t columnNum, enum DataType *types, const char **names) {
//
//    struct TableHeader *tableHeader = NULL;
//    getTable(&tableHeader, name);
//    if (tableHeader == NULL) {
//        struct BlockHeader *block = NULL;
//        struct BlockHeader *blockWithMap = NULL;
//        int32_t err = allocTable(&block, name, columnNum, types, names);
//        if (err != SUCCESS) {
//            unmapPage(blockWithMap);
//            unmapPage(block);
//            return err;
//        }
//        err = getHashMap(&blockWithMap);
//        if (err != SUCCESS) {
//            unmapPage(blockWithMap);
//            unmapPage(block);
//            return err;
//        }
//        err = addTableInMap(blockWithMap, block);
//        if (err != SUCCESS) {
//            unmapPage(blockWithMap);
//            unmapPage(block);
//            return err;
//        }
//
//    }
//    freeTable(tableHeader);
//    return SUCCESS;
//}
//
//int32_t deleteTableByHeader(struct TableHeader *tableHeader) {
//    struct BlockCoordinate rowPageBlockCoordinate = tableHeader->firstRowPage;
//    while (isValidPosition(rowPageBlockCoordinate.offset)) {
//        struct BlockHeader *rowPageBlock;
//        checkForErr(getBlockByOffsetAndSize(&rowPageBlock, rowPageBlockCoordinate.offset,
//                                            rowPageBlockCoordinate.size))
//        struct RowPage *rowPage = getBlockData(rowPageBlock);
//        rowPageBlockCoordinate = rowPage->nextRowPage;
//        freePage(rowPageBlock);
//    }
//    struct BlockHeader *blockWithMap;
//    checkForErr(getHashMap(&blockWithMap));
//
//    removeTableFromMap(blockWithMap, convertTableHeaderToBlockHeader(tableHeader));
//    unmapPage(blockWithMap);
//
//    freePage(convertTableHeaderToBlockHeader(tableHeader));
//    return SUCCESS;
//}
//
//int32_t deleteTable(const char *name) {
//    struct BlockHeader *table;
//    checkForErr(getTableAndReturnBlock(&table, name))
//    checkForErr(deleteTableByHeader(convertFileNodeToTableHeader(convertBlockHeaderToFileNode(table))))
//    return SUCCESS;
//}
//
//
//int32_t closeDatabaseFile() {
//    unmapPage(headerBlock);
//    closeAllocator();
//    return SUCCESS;
//}
//
//static int32_t initEmptyDatabase(void) {
//    struct BlockHeader *databaseHeaderBlock;
//
//    checkForErr(allocatePageInFile(&databaseHeaderBlock, sizeof(struct DatabaseHeader)))
//
//    struct BlockHeader *block;
//    int32_t err = initFreeFileMapAndReturnBlock(&block, KEY_STRING);
//    if(err != SUCCESS) {
//        unmapPage(databaseHeaderBlock);
//        return err;
//    }
//    struct DatabaseHeader *databaseHeader = getBlockData(databaseHeaderBlock);
//    databaseHeader->size = 0;
//    databaseHeader->tableMapPage = (struct BlockCoordinate) {block->offset, block->size};
//    databaseHeader->freeList = INVALID_HEADER_BLOCK_COORDINATE;
//    unmapPage(block);
//    headerBlock = databaseHeaderBlock;
//    header = databaseHeader;
//    return SUCCESS;
//}
//
//int32_t openDatabaseFileOrCreate(const char *path) {
//    checkForErr(initAllocator(path));
//    if (fileSize() != 0) {
//        struct BlockHeader *block;
//        checkForErr(getAllBlockByOffset(&block, 0))
//        header = getBlockData(block);
//    } else {
//        return initEmptyDatabase();
//    }
//    return SUCCESS;
//}
//struct BlockCoordinate getFreeList() {
//    return header == NULL ? INVALID_HEADER_BLOCK_COORDINATE : header->freeList;
//}
//void setFreeListHeader(struct BlockCoordinate bc) {
//    header->freeList = bc;
//}
//
//struct DatabaseHeader *getDatabaseHeader() {
//    return header;
//}
//
//
//
