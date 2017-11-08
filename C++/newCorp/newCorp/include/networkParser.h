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

using namespace std;

class networkV9TemplateContainer
{
private:
    char directory[MAX_CHARS_PER_LINE];
    vector <routerV9TemplateContainer*> ntContainer;
    netflowV9Container gTempCont;
public:
    virtual ~networkCfg();
    // init
    int initTemplateCSV(char* idir);
    void printTemplateAll(FILE* fb);

    //-----------netflow template
    netflowV9Container* getTemplateContainer() { return &gTempCont;}
    uint32_t sizeof() { return ntContainer.sizeof();}
};

#endif 
