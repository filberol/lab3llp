#include "../include/xml/from_xml_server.h"
#include "../include/net/public/server_socket_operations.h"
#include "../include/xml/to_xml_server.h"
#include "../include/instructions/file_instructions.h"
#include "../include/tables/table_schema_utils.h"
#include "../include/tables/table_row_utils.h"
#include "../include/public/table_structures.h"
#include <pthread.h>

FILE* db_file;

static void *handleClient(void *socket_desc) {
    int clientSocket = *(int *) socket_desc;
    free(socket_desc);
    struct TableHeader *selectTableHeader = NULL;
    struct RecordData *selectRecordData = NULL;

    while (1) {
        char *buffer = malloc(MAX_BUFFER_SIZE); //todo array
        int err;
        err = receiveData(clientSocket, buffer);
        if (err != 0) {
            printf("Unable to receive data");
            close(clientSocket);
            pthread_exit(NULL);
        }

        printf("Received data: %s\n", buffer);
        xmlDocPtr doc = xmlParseMemory(buffer, strlen(buffer));
        xmlDocPtr serverResponse = xmlNewDoc(BAD_CAST "1.0");
        xmlNodePtr xmlNodeSqlResp = xmlNewNode(NULL, BAD_CAST "sqlResponse");
        xmlDocSetRootElement(serverResponse, xmlNodeSqlResp);

        struct request request;
        err = parseXml(doc, &request);
        if (err != 0) {
            addMessage(xmlNodeSqlResp, "Unable to parse xml\n");
            close(clientSocket);
            pthread_exit(NULL);

        }
        printf("Request type: %d\n", request.type);
        if (request.type == CREATE_TABLE) {
            struct createTableRequest createTableRequest = request.createTableRequest;
            struct TableScheme table1scheme;
            err = init_table_scheme(db_file, createTableRequest.tableName, createTableRequest.columnNum, &table1scheme);
            for (int i = 1; i <= createTableRequest.columnNum; i++) {
                schema_set_column(db_file, &table1scheme, 1, (enum CellType) createTableRequest.types[i - 1], "id");
            }
            if (err != 0) {
                addMessage(xmlNodeSqlResp, "Unable to create table");
            }
            addMessage(xmlNodeSqlResp, "Table successfully created!");

        } else if (request.type == DROP_TABLE) {
            struct dropTableRequest dropTableRequest = request.dropTableRequest;
            struct StaticFileHeader header;
            read_static_header(db_file, &header);
            err = remove_table_index(header.table_indices_sector, db_file, dropTableRequest.tableName);
            if (err != 0) {
                addMessage(xmlNodeSqlResp, "Unable to drop table");
            }
            addMessage(xmlNodeSqlResp, "Table successfully dropped!");
        } else if (request.type == INSERT) {
            printf("STARTING TO READ TABLE\n");
            struct insertRequest insertRequest = request.insertRequest;
            // Find and read table
            struct StaticFileHeader header;
            read_static_header(db_file, &header);
            uint32_t schema_sector = find_table_sector(header.table_indices_sector, db_file, insertRequest.tableName);
            struct TableScheme table_schema;
            read_data_from_sector(db_file, &table_schema, sizeof(struct TableScheme), schema_sector);
            printf("READ THE TABLE DATA\n");
            if (schema_sector == 0) {
                addMessage(xmlNodeSqlResp, "Unable to find table");
            }
            // Set row
            union TableCellWithData table_row[table_schema.columns_count];
            struct RecordData *insertRecordData = prepareRecordDataStructure(&table_schema);
            for (int i = 0; i < insertRequest.dataCount; ++i) {
                insertRecordData->data[i] = insertRequest.data[i];
            }
            err = (int) add_row_to_file(db_file, &table_schema, table_row);
            if (err == 0) {
                addMessage(xmlNodeSqlResp, "Unable to insert");
            } else {
                addMessage(xmlNodeSqlResp, "Successfully inserted");
            }
        } else if (request.type == SELECT) {
            struct selectRequest selectRequest = request.selectRequest;
            struct StaticFileHeader header;
            read_static_header(db_file, &header);
            uint32_t schema_sector = find_table_sector(header.table_indices_sector, db_file, selectRequest.tableName);
            struct TableScheme table_schema;
            if (schema_sector == 0) {
                addMessage(xmlNodeSqlResp, "Unable to find table");
            }
            selectRecordData = prepareRecordDataStructure(&table_schema);
            addRecordData(xmlNodeSqlResp, selectRecordData);
            freeRecordData(selectRecordData);
            selectTableHeader = NULL;
            selectRecordData = NULL;
        } else if (request.type == DELETE) {
            struct deleteRequest deleteRequest = request.deleteRequest;
            struct StaticFileHeader header;
            read_static_header(db_file, &header);
            uint32_t schema_sector = find_table_sector(header.table_indices_sector, db_file, deleteRequest.tableName);
            struct TableScheme table_schema;
            read_data_from_sector(db_file, &table_schema, sizeof(struct TableScheme), schema_sector);
            if (schema_sector == 0) {
                addMessage(xmlNodeSqlResp, "Unable to find table");
            }
            err = delete_row_from_file(db_file, deleteRequest.conditionCount, &table_schema);
            if (err != 0) {
                addMessage(xmlNodeSqlResp, "Unable to delete");
            }
            addMessage(xmlNodeSqlResp, "Successfully deleted");
        }
        xmlChar *xmlBuffer = malloc(MAX_BUFFER_SIZE * sizeof(xmlChar));
        int size;
        xmlDocDumpFormatMemory(serverResponse, &xmlBuffer, &size, 1);
        printf("Response: %s\n", xmlBuffer);
        err = sendData(clientSocket, (char *) xmlBuffer);
        if (err != 0) {
            printf("Unable to send data");
        }
        freeRequest(&request);
        free(xmlBuffer);
        free(buffer);
    }
}

int main(int argc, char **argv) {
    printf("START\n");

    ++argv;
    --argc;
    if (argc != 3) {
        printf("Wrong arguments");
        return 1;
    }
    char *addr = argv[0];
    int port = atoi(argv[1]);
    char *filename = argv[2];
    int fd = initializeServerSocket(addr, port);
    if (fd < 0) {
        printf("Unable to initialize socket");
        return -fd;
    }
    int err = listenForConnections(fd);
    if (err != 0) {
        printf("Unable to listen for connections");
        return -err;
    }
    db_file = fopen(filename, "wb+");
    create_file_and_init_empty_structure(db_file);

    while (1) {
        printf("ACCEPTING CONNECTIONS\n");
        int clientSocket = acceptConnection(fd);
        if (clientSocket < 0) {
            printf("Unable to accept connection");
        }
        int *clientSocketPtr = malloc(sizeof(int));
        *clientSocketPtr = clientSocket;
        pthread_t tid;
        if (pthread_create(&tid, NULL, handleClient, (void *) clientSocketPtr) != 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
        pthread_detach(tid);
    }
}
