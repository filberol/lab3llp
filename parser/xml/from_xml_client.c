#include "from_xml_client.h"

static xmlNodePtr getChildByName(xmlNodePtr parent, const char *childName) {
    if (parent == NULL || childName == NULL) {
        return NULL;
    }
    for (xmlNodePtr child = parent->children; child != NULL; child = child->next) {
        if (child->type == XML_ELEMENT_NODE && xmlStrcmp(child->name, (const xmlChar *) childName) == 0) {
            return child;
        }
    }
    return NULL;
}

//returns 1 if not end
int parseResponse(xmlDocPtr xmlDocument) {
    xmlNodePtr rootNode = xmlDocGetRootElement(xmlDocument);
    xmlNodePtr msgNode = getChildByName(rootNode, "message");
    xmlNodePtr row = getChildByName(rootNode, "row");
    xmlNodePtr hasNext = getChildByName(rootNode, "hasNext");
    int res = 0;
    if (hasNext != NULL) {
        res = 1;
    }

    if (msgNode != NULL) {
        printf("Message: %s\n", msgNode->children->content);
    }
    if (row != NULL) {
        for(xmlNodePtr child = row->children; child != NULL; child = child->next) {
            if(xmlStrcmp(child->name, (const xmlChar *) "rowNode") == 0) {
                xmlNodePtr column = getChildByName(child, "name");
                xmlNodePtr val = getChildByName(child, "val");
                printf("%s: ", column->children->content);
                printf("%s\n", val->children->content);
            }
        }
    }
    return res;
}
