#ifndef routerV9TemplateCONTAINER_H
#define routerV9TemplateCONTAINER_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include "parserCommon.h"
#include <stdio.h>

// netflow v9 container
#include "netflow_v9.h"

// associate v9 template pointer with the source ID
// source ID + router ID + template ID => a unique template
struct routerV9Template
{
    u_int32_t 		sourceId;
    u_int16_t 		templateID;
    v9_template* 	v9TemplateObj;
};

class routerV9TemplateContainer
{
private:
  uint32_t  routerAddr;
  vector <routerV9Template*> srcTempContainer;

public:
  virtual ~routerV9TemplateContainer();
  routerV9Template* addTemplate (u_int32_t sID, u_int16_t tID, 
		const u_int16_t nf,
		const u_int16_t* fID,
		const u_int16_t* len,
		const bool option);
  routerV9Template* getrouterV9Template (u_int32_t sID, u_int16_t tID);
  void printContent();
  void printContent(FILE* fb);
  int   size() { return srcTempContainer.size();}
  uint32_t getRouterAddr() { return routerAddr;}
  void setRouterAddr(u_int32_t addr) { routerAddr = addr;}
};
#endif
