#ifndef MPLSFORWARDTABLE_H
#define MPLSFORWARDTABLE_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <stdio.h>
#include "parserCommon.h"

struct MFTdata
{
    uint32_t localLabel;
    int32_t outLabel;
	/* NOTE: -1 POP
		 -2 NO Label; Unlabelled;
		 -3 AGGREGATE */
    char nextHop[48];
    char tunnelID[48];
    char outInterface[48];

    // Juniper P router
    int32_t outLabel2;
    char outInterface2[48];
    char nextHop2[48];
};

class mplsForwardTable
{
private:
    vector <MFTdata*> myMFTdata;
    MFTdata* currentJuniperObj;
public:
  virtual ~mplsForwardTable();
  void addMFTdata (char* c0, char* c1, char* c2, char* c3, char* c4);
  void createCiscoObj(routerVendor rt, char* buf);
  void printAll (char* rName, FILE* fb);
  void createJuniperObj (uint32_t localLabel);
  void closeJuniperObj () {currentJuniperObj = NULL;}
  void updateJuniperNextHop (char* c1, char* c3);
  void updateJuniperOutLabel(int32_t n1) ;
  void updateJuniperPrefixRoute(char* c1);

  MFTdata* getMFTdataByEPEaddr (uint32_t ePEaddr);
};

#endif
