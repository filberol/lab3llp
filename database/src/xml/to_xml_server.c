#include "../../include/xml/to_xml_server.h"

void addMessage(xmlNodePtr root, char *error) {
    printf("%s\n", error);
    xmlNodePtr errorNode = xmlNewChild(root, NULL, BAD_CAST "message", BAD_CAST error);
    xmlAddChild(root, errorNode);
}

static char* getColumnValueByType(struct Column *column, void *data) {
    char *value = malloc(100);
    switch (column->dataType) {
        case INT_TYPE:
            sprintf(value, "%d", *(int*)data);
            break;
        case BOOL_TYPE:
            sprintf(value, "%d", *(int*)data);
            break;
        case DOUBLE_TYPE:
            sprintf(value, "%f", *(double*)data);
            break;
        case STRING_TYPE:
            return data;
        case INVALID_TYPE:
            break;
    }
    return value;
}

static char* pointerToStr(void* ptr) {
    char* buf = malloc(100);
    sprintf(buf, "%p", ptr);
    return buf;
}
static void freeBufStrRepr(char *buf, enum DataType dataType) {
    if(dataType != STRING_TYPE) {
        free(buf);
    }
}
void addRecordData(xmlNodePtr root, struct RecordData *data) {
    xmlNodePtr row = xmlNewChild(root, NULL, BAD_CAST "row",  NULL);
    xmlNodePtr hasNext = xmlNewChild(root, NULL, BAD_CAST "hasNext",  BAD_CAST "true");

    xmlAddChild(root, row);
    xmlAddChild(root, hasNext);

    for (int32_t i = 0; i < data->columnNum; i++) {
        char* val = getColumnValueByType(data->columns[i], data->data[i]);
        xmlNodePtr rowNode = xmlNewChild(row, NULL, BAD_CAST "rowNode", NULL);
        xmlAddChild(row, rowNode);
        xmlNodePtr nameNode = xmlNewChild(rowNode, NULL, BAD_CAST "name", BAD_CAST data->columns[i]->name);
        xmlNodePtr valNode = xmlNewChild(rowNode, NULL, BAD_CAST "val", BAD_CAST val);
        xmlAddChild(rowNode, nameNode);
        xmlAddChild(rowNode, valNode);
        freeBufStrRepr(val, data->columns[i]->dataType);
    }

}
