#ifndef NETWORKPARSER_H
#define NETWORKPARSER_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <algorithm> 
#include "parserCommon.h"
#include <stdio.h>
#include "netflowV9Container.h"
#include "routerTemplateContainer.h"

using namespace std;

class networkV9TemplateContainer
{
private:
    char directory[MAX_CHARS_PER_LINE];
    vector <routerV9TemplateContainer*> ntContainer;
    netflowV9Container gTempCont;
public:
    virtual ~networkV9TemplateContainer();
    // init
    int initTemplateCSV(char* idir);
    void printTemplateAll(FILE* fb);

    //-----------netflow template
    netflowV9Container* getTemplateContainer() { return &gTempCont;}
    routerV9TemplateContainer* getRtContainer (uint32_t routerAddress);
    void insertRtContainer (routerV9TemplateContainer * rtCont);
    uint32_t size() { return ntContainer.size();}
};

#endif 
