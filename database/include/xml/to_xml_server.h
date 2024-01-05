//
// Created by egor on 24.12.23.
//

#ifndef LAB3_TO_XML_SERVER_H
#define LAB3_TO_XML_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <libxml/xmlwriter.h>
#include <libxml/tree.h>

#include <libxml/xmlschemas.h>
#include "../public/database.h"

void addMessage(xmlNodePtr root, char *error);

void addRecordData(xmlNodePtr root, struct RecordData *data);

#endif //LAB3_TO_XML_SERVER_H
